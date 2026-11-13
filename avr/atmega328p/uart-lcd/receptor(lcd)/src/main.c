#include "../lib/lcd.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define BUF_SIZE 64
#define LINEA_MAX 32

volatile char buffer[BUF_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;

void uart_init(void) {
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
    sei();
}

ISR(USART_RX_vect) {
    char dato = UDR0;
    uint8_t next = (head + 1) % BUF_SIZE;
    if (next != tail) {
        buffer[head] = dato;
        head = next;
    }
}

uint8_t uart_leer(char *dato) {
    if (head == tail) return 0;
    *dato = buffer[tail];
    tail = (tail + 1) % BUF_SIZE;
    return 1;
}

int main(void) {
    uart_init();
    lcd_init();

    char linea[LINEA_MAX];
    uint8_t idx = 0;

    while (1) {
        char c;
        if (uart_leer(&c)) {
            if (c == '\n' || c == '\r') {
                if (idx > 0) {
                    linea[idx] = '\0';
                    lcd_clear();
                    lcd_puts(linea);     // ← corregido
                    idx = 0;
                }
            } else {
                if (idx < LINEA_MAX - 1) {
                    linea[idx++] = c;
                }
            }
        }
    }
}