#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <avr/io.h>
#include <util/delay.h>

#define TIME 500

uint8_t getStatusB0(void);
uint8_t getStatusB1(void);
uint8_t cooperative_delay(uint16_t ms);
uint8_t changeMode(uint8_t mode);

#endif