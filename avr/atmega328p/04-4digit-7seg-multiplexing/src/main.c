#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>

#define TIME 2
volatile uint16_t ms = 0;
volatile uint8_t seconds = 0;
volatile uint8_t minutes = 1;
volatile uint8_t hours = 16;

const uint8_t seven_seg[10] = {
    0b11000000, 
    0b11111001, 
    0b10100100, 
    0b10110000, 
    0b10011001, 
    0b10010010,  
    0b10000010, 
    0b11111000, 
    0b10000000,  
    0b10010000, 
};



void timer0_init(void)
{
    DDRD = 0xFF;
    DDRB |= ((1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3)); 
    TCCR0A = (1<<WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);  
    TIMSK0 = (1<<OCIE0A);
    OCR0A = 124;
    sei();
}

void multiplexing(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{      
       PORTB |= (1<<PB0);
       PORTD = seven_seg[d0];
       _delay_ms(TIME);
       PORTB &= ~(1<<PB0);

       PORTB |= (1<<PB1);
       PORTD = seven_seg[d1];
       _delay_ms(TIME);
       PORTB &= ~(1<<PB1);

       PORTB |= (1<<PB2);
       PORTD = seven_seg[d2];
       _delay_ms(TIME);
       PORTB &= ~(1<<PB2);

       PORTB |= (1<<PB3);
       PORTD = seven_seg[d3];
       _delay_ms(TIME);
       PORTB &= ~(1<<PB3);
}


int main(void)
{

    timer0_init();

    while(1){
     uint8_t h, m;
        cli();
        h = hours;
        m = minutes;
        sei();

        uint8_t d0 = h / 10;
        uint8_t d1 = h % 10;
        uint8_t d2 = m / 10;
        uint8_t d3 = m % 10;

        multiplexing(d0, d1, d2, d3);
    }
}



ISR(TIMER0_COMPA_vect)
{
    ms++;
    if (ms >= 1000)
    {
        ms = 0;
        seconds++;
        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;
            if (minutes >= 60)
            {
                minutes = 0;
                hours++;
                if (hours >= 24)
                    hours = 0;
            }
        }
    }
}