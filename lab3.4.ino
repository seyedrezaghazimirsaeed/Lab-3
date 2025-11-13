#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#define BUTTON_PIN 2   // INT0 (PD2)
#define POT_PIN    A0  // ADC0

volatile bool wakeFlag = false;

// Interrupt Service Routine for INT0
ISR(INT0_vect) {
  wakeFlag = true;   // Set flag when woken up
}

void setup() {
  // Disable ADC, peripherals, and set sleep mode to save power initially
  ADCSRA = 0; // ADC off
  power_all_disable();

  // Configure INT0 (external interrupt)
  EICRA |= (1 << ISC01);  // Falling edge triggers interrupt
  EIMSK |= (1 << INT0);   // Enable INT0 interrupt

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Pull-up for button

  sei(); // Enable global interrupts
}

void enterDeepSleep() {
  // Disable ADC, Timer, SPI, TWI, etc.
  ADCSRA &= ~(1 << ADEN); // ADC off
  power_all_disable();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Ensure interrupts are enabled before sleeping
  sei();
  sleep_cpu();

  // MCU continues here after waking up
  sleep_disable();
  power_all_enable();  // Re-enable peripherals
}

uint16_t readAnalogValue() {
  // Re-enable ADC
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Prescaler 64
  ADMUX = (1 << REFS0);  // AVcc reference
  ADMUX |= 0;            // ADC0 (A0)

  // Start conversion
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC)); // Wait for completion

  uint16_t result = ADC;

  ADCSRA &= ~(1 << ADEN); // Turn off ADC again to save power
  return result;
}

void loop() {
  if (!wakeFlag) {
    enterDeepSleep();
  }

  // --- MCU woke up here ---
  wakeFlag = false;

  // Read analog input (potentiometer)
  uint16_t potValue = readAnalogValue();

  // (Optional) simulate some brief processing
  delay(100); // pretend to handle data

  // Return to deep sleep
}