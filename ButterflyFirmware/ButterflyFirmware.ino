#define LEDS_TOTAL 36

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define PIN_ON 4

uint8_t leds[LEDS_TOTAL] = { 
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

void setup() {
  // set pins as outputs
  //DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
  pinMode(0 ,OUTPUT);
  pinMode(1 ,OUTPUT);
  pinMode(2 ,OUTPUT);
  pinMode(3 ,OUTPUT);
  pinMode(4, INPUT_PULLUP);
}


uint8_t loopCounter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  for(uint8_t idx = 0; idx < LEDS_TOTAL; idx++){
    PORTB = leds[idx];
    delay(10);
  }
  
  loopCounter++;

  if (loopCounter % 5 == 0){
    // sleep
    PORTB = 0x00;
    for(uint16_t idx = 0; idx < 120; idx++){
      goToSleep(); // each sleep takes 8sec
    }
    loopCounter = 0;
  }
}

void goToSleep() {

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
