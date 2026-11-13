#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void uart_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;                                 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);         
    UCSR0B = (1 << TXEN0);                          
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));             
    UDR0 = c;
}

void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}
int main(void) {
    uart_init();
    
    while (1) {
        uart_send_string("Hola mundo\r\n"); 
        _delay_ms(1000);
        
    }
}