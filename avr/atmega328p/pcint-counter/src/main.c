#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

volatile uint8_t counter = 7;

ISR(PCINT0_vect)
{
  if(PINB & (1<<PINB0))
  {
    _delay_ms(20);
    if(PINB & (1<<PINB0))
    {
      if(counter < 7) counter++; 
    }
  }   
  if(PINB & (1<<PINB1))
  {
    _delay_ms(20);
    if(PINB & (1<<PINB1))
    {
      if(counter > 0) counter--;
    }
  } 
}

int main()
{
    DDRD |= ((1<<PIND0) | (1<<PIND1) | (1<<PIND2) );
    DDRB &= ~((1<<PINB0) | (1<<PINB1));
    PCICR |= (1<<PCIE0);
    PCMSK0 |= ((1<<PCINT0) | (1<<PCINT1));
    sei();
    while(1)
    {
     PORTD = counter;
    }
}