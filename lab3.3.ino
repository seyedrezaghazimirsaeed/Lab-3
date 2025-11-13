#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define LED_PIN PB5          // Onboard LED on Arduino Uno

volatile uint8_t led_on = 0;
volatile uint8_t t0_count = 0;

// -------- Timer1: 1-second wake-up ----------
void timer1_init(void) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);               // CTC mode
    OCR1A = 15624;                        // 1 s at 16 MHz / 1024
    TIMSK1 |= (1 << OCIE1A);              // Enable compare interrupt
    TCCR1B |= (1 << CS12) | (1 << CS10);  // Start, prescaler = 1024
}

// -------- Timer0: 0.1-second LED timer -----
void timer0_start(void) {
    TCCR0A = (1 << WGM01);                // CTC
    TCCR0B = 0;
    OCR0A = 249;                          // 1 ms tick (16 MHz/64/250)
    TIMSK0 |= (1 << OCIE0A);              // Interrupt enable
    TCCR0B |= (1 << CS01) | (1 << CS00);  // Start, prescaler = 64
    t0_count = 0;
}

void timer0_stop(void) {
    TCCR0B = 0;
    TIMSK0 &= ~(1 << OCIE0A);
}

// -------- Interrupts -----------------------
ISR(TIMER1_COMPA_vect) {
    PORTB |= (1 << LED_PIN);   // LED ON
    led_on = 1;
    timer0_start();            // Start 0.1 s timer
}

ISR(TIMER0_COMPA_vect) {
    if (led_on) {
        t0_count++;
        if (t0_count >= 100) {     // ≈100 ms
            PORTB &= ~(1 << LED_PIN); // LED OFF
            led_on = 0;
            timer0_stop();
        }
    }
}

// -------- Main -----------------------------
int main(void) {
    DDRB |= (1 << LED_PIN);         // Output
    PORTB &= ~(1 << LED_PIN);       // LED OFF

    timer1_init();
    sei();                          // Enable interrupts
    set_sleep_mode(SLEEP_MODE_IDLE); // Use IDLE to keep Timer1 running

    while (1) {
        sleep_mode();               // Sleep until interrupt
    }
}