#define LED_SPRITES 36

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define PIN_ON 4

uint8_t ledSprite[LED_SPRITES] = { 
  B1110, B1101, B1110, B1101,
  B1110, B1101, B1011, B0111,
  B1011, B0111, B1011, B0111, 
   
  B0010, B0100, B0001, B0100, 
  B1000, B0001, B1000, B0001,
  B0010, B1000, B0010, B0100,
   
  B0110, B0101, B0110, B0101,
  B1010, B1001, B0011, B1011,
  B0011, B0110, B1001, B0101
};

#define LEDS_TOTAL 12
uint8_t led[LEDS_TOTAL] = {
  B0010, B0001, B0100, B0100,
  B1000, B1000, B0001, B0001, 
  B0010, B0010, B1000, B0100 
};
uint8_t ledMask[LEDS_TOTAL] = {
  B0011, B0011, B0101, B0110,
  B1001, B1010, B0101, B1001,
  B0110, B1010, B1100, B1100,
};


void setup() {
  // set pins as outputs
  //DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
  pinMode(0 ,OUTPUT);
  pinMode(1 ,OUTPUT);
  pinMode(2 ,OUTPUT);
  pinMode(3 ,OUTPUT);
  pinMode(4, INPUT);

}

void ledON(uint8_t idx) {
    DDRB = ledMask[idx];
    PORTB = led[idx];
}
void ledON(uint8_t idx, int duration) {
    ledON(idx);
    delay(duration);
}

void ledON_us(uint8_t idx, int duration) {
    ledON(idx);
    _delay_us(duration);
}

void rotate(int spd) {
  ledON(1, spd);
  ledON(3, spd);
  ledON(9, spd);
  ledON(5, spd);
  ledON(7, spd);
  ledON(6, spd);
  ledON(0, spd);
  ledON(8, spd);
  ledON(4, spd);
  ledON(2, spd);
}

void rotate_us(int spd_ms) {
  ledON_us(1, spd_ms);
  ledON_us(3, spd_ms);
  ledON_us(9, spd_ms);
  ledON_us(5, spd_ms);
  ledON_us(7, spd_ms);
  ledON_us(6, spd_ms);
  ledON_us(0, spd_ms);
  ledON_us(8, spd_ms);
  ledON_us(4, spd_ms);
  ledON_us(2, spd_ms);
}

void multiBlink() {
  for(uint8_t idx = 0; idx < LED_SPRITES; idx++){
    PORTB = ledSprite[idx];
    delay(10);
  }
}

unsigned long loopCounter = 0;

void loop() {
  for(uint8_t idx = 10; idx > 1; idx--){ rotate(idx); }
  for(uint8_t idx = 500; idx > 300; idx-=100){ rotate_us(idx); }
  rotate_us(100);
  rotate_us(100);

  DDRB = B1111;

  for(uint8_t idx = 10; idx > 1; idx--){ multiBlink(); }

  loopCounter++;

  // sleep
  PORTB = 0x00;
  for(uint16_t idx = 0; idx < loopCounter; idx++){
    goToSleep(); // each sleep takes 8sec
  }
}

void goToSleep() {
  pinMode(4, INPUT);

  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  /* set new watchdog timeout prescaler value */
  WDTCR |= (1<<WDCE) | (1<<WDE);
  WDTCR = 1<<WDP0 | 0<<WDP1 | 0<<WDP2 | 1<<WDP3;
  WDTCR |= _BV(WDIE);
  
    GIFR |= _BV(PCIF);                      // clear interupt flag
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT4);                   // Use PB4 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT4);                  // Turn off PB4 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
} // sleep

ISR(PCINT0_vect) {
// This is called when the interrupt occurs, but I don't need to do anything in it
    GIFR |= _BV(PCIF);                      // clear interupt flag
    PCMSK &= ~_BV(PCINT4);                  // Turn off PB4 as interrupt pin
}


//void goToSleep() {
//  /*** Setup the Watch Dog Timer ***/
//  /* Clear the reset flag. */
//  MCUSR &= ~(1<<WDRF);
//  
//  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
//  PCMSK |= _BV(PCINT4);                   // Use PB4 as interrupt pin
//    
//  /* set new watchdog timeout prescaler value */
//  WDTCR |= (1<<WDCE) | (1<<WDE);
//  WDTCR = 1<<WDP0 | 0<<WDP1 | 0<<WDP2 | 1<<WDP3;
//  WDTCR |= _BV(WDIE);
//
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//  sleep_enable();
//  sei(); 
//  
//  sleep_mode();
//  
//  sleep_disable();
//  PCMSK &= ~_BV(PCINT4);
//  sei(); 
//}



ISR(WDT_vect) {
  // Dummy watchdog timer handler to prevent reset
}

//ISR(PCINT0_vect) {
//  // This is called when the interrupt occurs, but I don't need to do anything in it
//  sleep_disable();
//  PCMSK &= ~_BV(PCINT4);
//}
