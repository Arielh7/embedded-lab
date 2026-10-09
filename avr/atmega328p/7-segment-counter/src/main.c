#include <avr/io.h>
#include <util/delay.h>


#define getStatusB0() ((PINB & (1 << PB0)) ? 1 : 0)


const uint8_t seven_seg[10] = {
    0b00000011, 
    0b10011111, 
    0b00100101, 
    0b00001101, 
    0b10011001, 
    0b01001001, 
    0b01000001, 
    0b00011111, 
    0b00000000,  
    0b00001001,
};

uint8_t counter = 0;
uint8_t b0Bef = 0;

int main(void)
{
    DDRD = 0xFF;
    DDRB = 0x00;
    PORTB &= ~(1 << PB0);    

    while (1)
    {
        PORTD = seven_seg[counter];

        uint8_t b0Now = getStatusB0();   


        if (b0Now == 1 && b0Bef == 0)
        {
            _delay_ms(20);            
            if (getStatusB0())      
            {
                counter++;
                if (counter > 9) counter = 0;
            }
        }

        b0Bef = b0Now;                   
    }
}