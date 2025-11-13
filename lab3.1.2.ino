/*
 * ATmega328P normal mode LED blink (no sleep)
 * LED on PB5 (Arduino digital pin 13)
 */

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    DDRB |= (1 << PB5);  // Set PB5 as output

    while (1) {
        PORTB |= (1 << PB5);   // LED ON
        _delay_ms(100);        // ON for 0.1 s

        PORTB &= ~(1 << PB5);  // LED OFF
        _delay_ms(900);        // OFF for 0.9 s
    }
}