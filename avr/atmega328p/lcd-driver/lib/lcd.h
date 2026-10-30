#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* Pin mapping - change here if wiring changes */
#define LCD_PORT   PORTD
#define LCD_DDR    DDRD
#define LCD_RS     PD0
#define LCD_E      PD1
#define LCD_D4     PD2   /* nibble goes on PD2..PD5 */

/* Panel size */
#define LCD_COLS   16
#define LCD_ROWS   2

/* HD44780 command set */
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_HOME            0x02
#define LCD_CMD_ENTRY_INC       0x06
#define LCD_CMD_ENTRY_DEC       0x04
#define LCD_CMD_DISPLAY_ON      0x0C
#define LCD_CMD_DISPLAY_OFF     0x08
#define LCD_CMD_CURSOR_ON       0x0E
#define LCD_CMD_CURSOR_BLINK    0x0F
#define LCD_CMD_FUNCTION_4BIT   0x28  /* 4-bit bus, 2 lines, 5x8 font */
#define LCD_CMD_SHIFT_LEFT      0x18
#define LCD_CMD_SHIFT_RIGHT     0x1C

/* Public API */

void lcd_init(void);

/* Basic write */
void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data(uint8_t data);

/* Position */
void lcd_clear(void);
void lcd_home(void);
void lcd_goto(uint8_t row, uint8_t col);

/* Display control */
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_cursor_on(void);
void lcd_cursor_off(void);
void lcd_blink_on(void);
void lcd_blink_off(void);

/* Shifting */
void lcd_shift_left(void);
void lcd_shift_right(void);
void lcd_scroll_left(void);
void lcd_scroll_right(void);

/* Number printing */
void lcd_print_uint(uint16_t num);
void lcd_print_int(int16_t num);
void lcd_print_hex(uint8_t num);
void lcd_print_float(float num, uint8_t decimals);
void lcd_fill(char c, uint8_t count);

#endif /* LCD_H */