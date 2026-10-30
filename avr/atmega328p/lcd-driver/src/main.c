#include "../lib/lcd.h"
#include <util/delay.h>

int main(void)
{
    lcd_init();

    lcd_puts("Hello World!");
    lcd_goto(1, 0);
    lcd_puts("LCD OK");

    _delay_ms(2000);

    lcd_clear();
    lcd_puts("Values to show:");
    lcd_goto(1, 0);
    lcd_print_int(-1234);
    lcd_putc(' ');
    lcd_print_hex(0xAA);
    lcd_putc(' ');
    lcd_print_float(3.1416f, 3);

    while (1) { }
}