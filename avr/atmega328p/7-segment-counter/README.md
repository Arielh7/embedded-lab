# 7-Segment Counter

AVR firmware that drives a single 7-segment display as a decimal counter
(0–9). A push button increments the count. Debouncing and edge detection
are implemented in software.

## Features

- Single 7-segment display (common anode)
- Push button increments the counter (0 → 9 → 0)
- Rising-edge detection with 20 ms debounce
- Written in pure C (no Arduino framework)
- Target: ATmega328P @ 8 MHz
- PlatformIO project

## Hardware

| Component         | Connection      |
| ----------------- | --------------- |
| 7-segment display | PORTD (PD0–PD7) |
| Common anode      | VCC (5V)        |
| Push button B0    | PB0             |
| Pull-down B0      | 10kΩ to GND     |
| VCC               | 5V              |
| GND               | Common ground   |

### Wiring diagram (text)

```
              ATmega328P
            ┌───────────┐
   Seg a ───┤ PD0   PB0 ├──── B0 ──┬── VCC
   Seg b ───┤ PD1       │          │
   Seg c ───┤ PD2       │         [10k]
   Seg d ───┤ PD3       │          │
   Seg e ───┤ PD4       │         GND
   Seg f ───┤ PD5       │
   Seg g ───┤ PD6       │
   Seg dp ──┤ PD7       │
            └───────────┘
```

The display is **common anode**: the common pin goes to VCC, and each
segment is turned on by pulling its pin LOW (0 = on).

Segment bit mapping (PORTD):

| Bit | 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Seg | a   | b   | c   | d   | e   | f   | g   | dp  |

## How it works

1. The main loop writes `seven_seg[counter]` to PORTD, which lights up
   the corresponding digit.
2. Each iteration reads the button state and compares it with the previous
   state to detect a rising edge (0 → 1).
3. When an edge is detected, a 20 ms debounce delay is applied, and the
   button is read again to confirm the press.
4. If confirmed, the counter increments (wrapping from 9 to 0).
5. The previous button state is updated at the end of every loop
   iteration, so the next edge can be detected.

## Digit table

The `seven_seg[]` array maps each decimal digit to the byte that must be
written to PORTD:

```c
const uint8_t seven_seg[10] = {
    0b00000011,  // 0
    0b10011111,  // 1
    0b00100101,  // 2
    0b00001101,  // 3
    0b10011001,  // 4
    0b01001001,  // 5
    0b01000001,  // 6
    0b00011111,  // 7
    0b00000001,  // 8
    0b00001001,  // 9
};
```

## Build and flash

### With PlatformIO

```bash
pio run              # build
pio run --target upload   # flash
```

`platformio.ini`:

```ini
[env:ATmega328P]
platform = atmelavr
board = ATmega328P
framework = arduino
board_build.f_cpu = 8000000L
```

### With avr-gcc / avrdude

```bash
avr-gcc -mmcu=atmega328p -DF_CPU=8000000UL -Os \
    src/main.c -o main.elf
avr-objcopy -O ihex main.elf main.hex
avrdude -c usbasp -p m328p -U flash:w:main.hex
```

## Simulation (Proteus)

1. Place an ATmega328P in the schematic.
2. Double-click the MCU and set **Clock Frequency** to match `F_CPU`
   (8 MHz). If it doesn't match, `_delay_ms()` will not behave correctly.
3. Connect the display segments to PORTD (with resistors) and the
   common anode to VCC.
4. Connect the button to PB0 with a 10kΩ pull-down to GND.
5. Load the `.hex` file into the MCU's program memory.
6. Run the simulation.

> Note: Proteus uses ideal switches with no mechanical bounce.
> The debounce logic will still work, but you won't see its effect
> in the simulator.

## File structure

```
7-segment-counter/
├── src/
│   └── main.c        # Main loop, counter logic, digit table
├── lib/              # PlatformIO private libraries
├── test/             # PlatformIO tests
├── platformio.ini
├── LICENSE
└── README.md
```

## Troubleshooting

| Symptom                                | Cause                      | Fix                            |
| -------------------------------------- | -------------------------- | ------------------------------ |
| Counter doesn't advance                | Floating input             | Add 10kΩ pull-down to GND      |
| Counter jumps multiple times per press | Missing debounce           | Increase debounce to 30–50 ms  |
| Counter only advances once             | Previous state not updated | Set `b0Bef = b0Now` every loop |
| Wrong digit displayed                  | Wrong segment mapping      | Verify `seven_seg[]` values    |
| `_delay_ms` wrong timing               | `F_CPU` mismatch           | Match `F_CPU` with MCU clock   |

## License

MIT
