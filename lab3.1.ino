#define F_CPU 16000000UL      // MCU frequency (e.g., 16 MHz)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void timer1_init(void) {
    // Set Timer1 to CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << WGM12);

    // Set compare value for 1 second interval
    // Formula: OCR1A = (F_CPU / (Prescaler * Frequency)) - 1
    // => OCR1A = (16,000,000 / (1024 * 1)) - 1 = 15624
    OCR1A = 15624;

    // Enable Output Compare A Match Interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Set prescaler to 1024 and start timer
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

void setup(void) {
    // Configure LED pin as output (PB0)
    DDRB |= (1 << PB0);
    PORTB &= ~(1 << PB0);    // Turn LED off initially

    // Initialize Timer1
    timer1_init();

    // Enable global interrupts
    sei();

    // Configure sleep mode (Power-down)
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop(void) {
    // Enter sleep mode
    sleep_mode();  // CPU sleeps here until Timer1 interrupt wakes it up
}

// Timer1 Compare Match Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
    // Wake up and blink LED
    PORTB |= (1 << PB0);     // LED ON
    _delay_ms(100);          // Wait 0.1s
    PORTB &= ~(1 << PB0);    // LED OFF
}
