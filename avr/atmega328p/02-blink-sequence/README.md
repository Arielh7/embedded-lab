# LED Sequence Controller with Button Modes

A bare-metal AVR project (ATmega328P) that controls 8 LEDs through 3 different
animation sequences, selectable via two push buttons. It implements a
non-blocking cooperative delay with edge detection and debouncing.

## Features

- 3 LED animation sequences (A, B, C)
- 2 operating modes: manual and automatic
- Cooperative delay: buttons are polled every 1 ms
- Edge detection (rising edge) with 20 ms debounce
- No blocking delays in the main loop
- Written in pure C (no Arduino framework)

## Hardware

| Component    | Connection      |
| ------------ | --------------- |
| 8 LEDs       | PORTD (PD0–PD7) |
| Button B0    | PB0             |
| Button B1    | PB1             |
| Pull-down B0 | 10kΩ to GND     |
| Pull-down B1 | 10kΩ to GND     |
| VCC          | 5V              |
| GND          | Common ground   |

Each LED must have a current-limiting resistor (220Ω–330Ω) in series.

## How it works

### Manual mode (default)

- The current sequence runs in a loop.
- Press **B0** to cycle through sequences: 1 → 2 → 3 → 1.
- Press **B1** to switch to automatic mode.

### Automatic mode

- The current sequence runs once.
- When it finishes, the controller advances to the next sequence automatically.
- Press **B1** to return to manual mode.

## Build and flash

### With PlatformIO

```bash
pio run              # build
pio run --target upload   # flash
```
