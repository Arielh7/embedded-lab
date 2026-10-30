
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define TRAFFIC_MACHINE PORTD
#define RED_LIGHT       PD0
#define GREEN_LIGHT     PD1
#define YELLOW_LIGHT    PD2

#define DATA_PIN        PB0
#define CK_PIN          PB1
#define SLACH_PIN       PB2

#define NUM_PHASES      3
#define TICKS_PER_SEC   1    


typedef struct {
    uint8_t light_mask;  
    uint8_t duration;     
} phase_t;


static const phase_t phases[NUM_PHASES] = {
    { (1 << RED_LIGHT),    9 },   
    { (1 << GREEN_LIGHT),  9 },
    { (1 << YELLOW_LIGHT), 3 },  
};


static const uint8_t seven_seg[10] = {
    0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001,
    0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001,
};


volatile uint8_t current_phase = 0;   
volatile uint8_t seconds_left  = 0;   
volatile uint8_t display_dirty = 1;   


static void shift_out(uint8_t data)
{
    PORTB &= ~(1 << SLACH_PIN);
    for (int8_t i = 7; i >= 0; i--) {
        if (data & (1 << i)) PORTB |=  (1 << DATA_PIN);
        else                 PORTB &= ~(1 << DATA_PIN);
        PORTB |=  (1 << CK_PIN);
        _delay_us(1);
        PORTB &= ~(1 << CK_PIN);
        _delay_us(1);
    }
    PORTB |= (1 << SLACH_PIN);
}


static void apply_phase(uint8_t idx)
{
    
    uint8_t mask = phases[idx].light_mask;
    TRAFFIC_MACHINE = (TRAFFIC_MACHINE & ~((1<<RED_LIGHT)|(1<<GREEN_LIGHT)|(1<<YELLOW_LIGHT))) | mask;

    seconds_left  = phases[idx].duration;
    display_dirty = 1;
}

ISR(TIMER1_COMPA_vect)
{
    if (seconds_left > 1) {
        seconds_left--;
        display_dirty = 1;
    } else {
        current_phase = (current_phase + 1) % NUM_PHASES;
        apply_phase(current_phase);
    }
}


int main(void)
{
  
    DDRD |= (1<<RED_LIGHT) | (1<<GREEN_LIGHT) | (1<<YELLOW_LIGHT);
    DDRB |= (1<<DATA_PIN)  | (1<<CK_PIN)       | (1<<SLACH_PIN);

  
    cli();
    TCCR1A = 0;
    TCCR1B = (1<<WGM12) | (1<<CS12);
    OCR1A  = 62499;
    TIMSK1 = (1<<OCIE1A);
    TCNT1  = 0;


    apply_phase(0);
    sei();

    uint8_t last_value = 0xFF;
    while (1) {
        if (display_dirty) {
            display_dirty = 0;
            if (seconds_left != last_value) {
                last_value = seconds_left;
                shift_out(seven_seg[seconds_left]);
            }
        }
    }
}