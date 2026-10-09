#include "sequence.h"

int main(void)
{
    DDRD = 0xFF;
    DDRB = 0x00;
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);

    uint8_t sequence = 1;
    uint8_t mode = 0;

    while (1)
    {
        uint8_t interrupted = changeMode(sequence);
        if (interrupted)
        {
            _delay_ms(20);
            if (getStatusB0())
            {
                _delay_ms(20);
                if (getStatusB0())
                {
                    sequence++;
                    if (sequence > 3) sequence = 1;

                    while (getStatusB0());
                    _delay_ms(20);
                }
            }
            if (getStatusB1())
            {
                _delay_ms(20);
                if (getStatusB1())
                {
                    mode = !mode;

                    while (getStatusB1());
                    _delay_ms(20);
                }
            }
        }
        else
        {
            if (mode == 1)
            {
                sequence++;
                if (sequence > 3) sequence = 1;
            }
        }
    }

    return 0;
}