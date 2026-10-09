#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT0_vect)
{
    PORTB ^= (1<<PINB3);
}


int main()
{
    DDRB |= ((1<<PINB0) | (1<<PINB1) | (1<<PINB2) | (1<<PINB3));
    DDRD &= ~(1<<PIND2);
    EICRA |= ((1<<ISC00) | (1<<ISC01));
    EIMSK |= (1 << INT0); 
    sei();

    while(1)
    {
        PORTB |= (1<<PINB0);
        _delay_ms(500);
        PORTB |= (1<<PINB1);
        _delay_ms(500);
        PORTB |= (1<<PINB2);
        _delay_ms(500);
        PORTB &= ~(1<<PINB0);
        _delay_ms(500);
        PORTB &= ~(1<<PINB1);
        _delay_ms(500);
        PORTB &= ~(1<<PINB2);
        _delay_ms(500);
    }
}