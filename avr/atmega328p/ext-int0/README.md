# ATmega328P External Interrupt (INT0) Demo

A bare-metal AVR project that demonstrates how to use an **external interrupt (INT0)** on the ATmega328P. A push button triggers the interrupt, which toggles an LED, while the main loop runs an independent LED sequence — proving that the interrupt executes asynchronously without blocking the main program.

## Features

- Bare-metal C (no Arduino framework)
- External interrupt on **INT0** (PD2 / Arduino pin 2)
- Rising-edge trigger
- ISR toggles an LED on **PB3** (Arduino pin 11)
- Main loop runs a 3-LED sequence on PB0, PB1, PB2
- Direct register manipulation (`DDRx`, `PORTx`, `EICRA`, `EIMSK`, `EIFR`)

## Hardware

### Components

- 1x ATmega328P (or Arduino Uno as a target)
- 4x LEDs
- 4x 280Ω resistors (for LEDs)
- 1x push button
- 1x 10kΩ resistor (pull-down)
- Breadboard and jumper wires

### Pin Connections

| Signal        | ATmega328P Pin | Arduino Pin | Notes                         |
| ------------- | -------------- | ----------- | ----------------------------- |
| LED0          | PB0            | 8           | Through 280Ω to GND           |
| LED1          | PB1            | 9           | Through 280Ω to GND           |
| LED2          | PB2            | 10          | Through 280Ω to GND           |
| LED_INT (PB3) | PB3            | 11          | Through 280Ω to GND           |
| Button        | PD2 (INT0)     | 2           | To VCC; 10kΩ pull-down to GND |

### Button Wiring (Pull-Down)
