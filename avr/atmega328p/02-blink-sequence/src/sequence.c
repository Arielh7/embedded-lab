#include "sequence.h"

uint8_t getStatusB0(void)
{
    return (PINB & (1 << PB0)) ? 1 : 0;
}

uint8_t getStatusB1(void)
{
    return (PINB & (1 << PB1)) ? 1 : 0;
}

uint8_t cooperative_delay(uint16_t ms)
{
    static uint8_t b0Previous = 0;
    static uint8_t b1Previous = 0;

    for (uint16_t i = 0; i < ms; i++)
    {
        _delay_ms(1);

        uint8_t b0Now = getStatusB0();
        uint8_t b1Now = getStatusB1();

        if ((b0Now == 1 && b0Previous == 0) ||
            (b1Now == 1 && b1Previous == 0))
        {
            _delay_ms(20);
            b0Previous = b0Now;
            b1Previous = b1Now;
            return 1;
        }

        b0Previous = b0Now;
        b1Previous = b1Now;
    }

    return 0;
}

static uint8_t sequence_A(void)
{
    PORTD = 0b11110000;
    if (cooperative_delay(TIME)) return 1;
    PORTD = 0b00001111;
    if (cooperative_delay(TIME)) return 1;
    return 0;
}

static uint8_t sequence_B(void)
{
    PORTD = 0b10101010;
    if (cooperative_delay(TIME)) return 1;
    PORTD = 0b01010101;
    if (cooperative_delay(TIME)) return 1;
    return 0;
}

static uint8_t sequence_C(void)
{
    PORTD = 0x00;

    for (uint8_t c = 0; c < 4; c++)
    {
        PORTD |= (1 << c) | (1 << (7 - c));
        if (cooperative_delay(TIME)) return 1;
    }

    for (uint8_t c = 4; c > 0; c--)
    {
        PORTD &= ~((1 << (c - 1)) | (1 << (8 - c)));
        if (cooperative_delay(TIME)) return 1;
    }

    return 0;
}

uint8_t changeMode(uint8_t mode)
{
    switch (mode)
    {
        case 1:  return sequence_A();
        case 2:  return sequence_B();
        default: return sequence_C();
    }
}