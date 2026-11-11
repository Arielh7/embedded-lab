#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t adc_result = 0;

void pwm_init(void)
{
    DDRD |= (1 << PD6); 
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);
}

void adc_init_free_running(void)
{
    ADMUX  = (1 << REFS0);         
    ADCSRB = 0;                     
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADSC);
}

int main(void)
{
    DDRB  = 0xFF;                                  
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD6);  
    pwm_init();
    adc_init_free_running();
    sei();

    while (1)
    {
        PORTB = adc_result & 0xFF;

  
        if (adc_result & (1 << 8)) PORTD |=  (1 << PD0);
        else                       PORTD &= ~(1 << PD0);

        if (adc_result & (1 << 9)) PORTD |=  (1 << PD1);
        else                       PORTD &= ~(1 << PD1);
    }
}
ISR(ADC_vect)
{
    adc_result = ADC;            
    OCR0A      = adc_result >> 2;   
}