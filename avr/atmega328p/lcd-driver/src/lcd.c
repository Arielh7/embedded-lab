#include "../lib/lcd.h"
#include <avr/io.h>
#include <util/delay.h>

/* --- low level helpers --- */

static void lcd_send_nibble(uint8_t nibble)
{
    /* clear PD2..PD5 */
    LCD_PORT &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5));

    /* bit0->PD2, bit1->PD3, bit2->PD4, bit3->PD5 */
    LCD_PORT |= ((nibble & 0x0F) << PD2);

    /* enable pulse */
    LCD_PORT |=  (1 << LCD_E);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_E);
    _delay_us(50);
}

static void lcd_write_byte(uint8_t rs, uint8_t data)
{
    if (rs) LCD_PORT |=  (1 << LCD_RS);
    else    LCD_PORT &= ~(1 << LCD_RS);

    lcd_send_nibble(data >> 4);     /* high nibble first */
    lcd_send_nibble(data & 0x0F);   /* then low nibble */
}

/* --- public API --- */

void lcd_init(void)
{
    LCD_DDR |= (1 << LCD_RS) | (1 << LCD_E) |
               (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);

    _delay_ms(50);

    LCD_PORT &= ~(1 << LCD_RS);

    /* 4-bit init sequence per HD44780U datasheet */
    lcd_send_nibble(0x03); _delay_ms(5);
    lcd_send_nibble(0x03); _delay_us(150);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02); _delay_us(150);

    lcd_write_cmd(LCD_CMD_FUNCTION_4BIT);
    lcd_write_cmd(LCD_CMD_DISPLAY_ON);
    lcd_write_cmd(LCD_CMD_CLEAR);        _delay_ms(2);
    lcd_write_cmd(LCD_CMD_ENTRY_INC);
}

void lcd_write_cmd(uint8_t cmd)  { lcd_write_byte(0, cmd);  _delay_us(50); }
void lcd_write_data(uint8_t data){ lcd_write_byte(1, data); _delay_us(50); }

void lcd_putc(char c)  { lcd_write_byte(1, (uint8_t)c); }

void lcd_puts(const char *s)
{
    while (*s) lcd_putc(*s++);
}

void lcd_clear(void)  { lcd_write_cmd(LCD_CMD_CLEAR); _delay_ms(2); }
void lcd_home(void)   { lcd_write_cmd(LCD_CMD_HOME);  _delay_ms(2); }

void lcd_goto(uint8_t row, uint8_t col)
{
    /* row 0 -> 0x00, row 1 -> 0x40 */
    uint8_t addr = (row ? 0x40 : 0x00) + col;
    lcd_write_cmd(0x80 | addr);
}

void lcd_display_on(void)  { lcd_write_cmd(LCD_CMD_DISPLAY_ON);  }
void lcd_display_off(void) { lcd_write_cmd(LCD_CMD_DISPLAY_OFF); }
void lcd_cursor_on(void)   { lcd_write_cmd(0x0E); }
void lcd_cursor_off(void)  { lcd_write_cmd(0x0C); }
void lcd_blink_on(void)    { lcd_write_cmd(LCD_CMD_CURSOR_BLINK); }
void lcd_blink_off(void)   { lcd_write_cmd(LCD_CMD_CURSOR_ON); }

void lcd_shift_left(void)  { lcd_write_cmd(LCD_CMD_SHIFT_LEFT);  }
void lcd_shift_right(void) { lcd_write_cmd(LCD_CMD_SHIFT_RIGHT); }
void lcd_scroll_left(void) { lcd_write_cmd(0x18); }
void lcd_scroll_right(void){ lcd_write_cmd(0x1C); }

/* --- number printing (no sprintf, saves flash) --- */

void lcd_print_uint(uint16_t num)
{
    char buf[6];
    int8_t i = 0;

    if (num == 0) { lcd_putc('0'); return; }

    while (num > 0) { buf[i++] = '0' + (num % 10); num /= 10; }
    while (i--) lcd_putc(buf[i]);
}

void lcd_print_int(int16_t num)
{
    if (num < 0) { lcd_putc('-'); num = -num; }
    lcd_print_uint((uint16_t)num);
}

void lcd_print_hex(uint8_t num)
{
    const char hex[] = "0123456789ABCDEF";
    lcd_putc(hex[(num >> 4) & 0x0F]);
    lcd_putc(hex[num & 0x0F]);
}

void lcd_print_float(float num, uint8_t decimals)
{
    if (num < 0) { lcd_putc('-'); num = -num; }

    uint32_t mult = 1;
    for (uint8_t i = 0; i < decimals; i++) mult *= 10;

    uint32_t scaled = (uint32_t)(num * mult + 0.5f);
    uint32_t int_part = scaled / mult;
    uint32_t frac_part = scaled % mult;

    lcd_print_uint((uint16_t)int_part);

    if (decimals > 0) {
        lcd_putc('.');
        /* pad fractional part with leading zeros */
        uint32_t div = mult / 10;
        while (div > 0) {
            lcd_putc('0' + (frac_part / div) % 10);
            div /= 10;
        }
    }
}

void lcd_fill(char c, uint8_t count)
{
    while (count--) lcd_putc(c);
}