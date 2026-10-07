#include <avr/io.h>

void adc_init() {
    ADMUX = (1 << REFS0);  
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    (void)ADC;
}

uint16_t adc_read() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main()
{
    DDRD |= (1 << PD6);
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);

    adc_init();

    uint16_t adc_filtrado = 0;

    while(1)
    {
        uint16_t lectura = adc_read();
        adc_filtrado = (adc_filtrado * 7 + lectura) / 8;

        uint8_t pwm_val = (uint8_t)(adc_filtrado >> 2);
        if (pwm_val < 15) pwm_val = 0;   

        OCR0A = pwm_val;
    }
    return 0;
}