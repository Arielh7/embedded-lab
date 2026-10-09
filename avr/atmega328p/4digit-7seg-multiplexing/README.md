# 4-Digit 7-Segment Multiplexing Clock

AVR firmware that drives a 4-digit 7-segment display as a real-time
clock (HH:MM) using time-division multiplexing. The time is kept by
Timer0 in CTC mode with a 1 ms interrupt.

## Features

- 4-digit 7-segment display (common anode)
- Time-division multiplexing: only 4 MCU pins drive the 4 digits
- Timer0 CTC interrupt at 1 ms for accurate timekeeping
- Hours (0–23) and minutes (0–59) tracked in software
- Target: ATmega328P @ 8 MHz
- PlatformIO project

## Hardware

| Component            | Connection           |
| -------------------- | -------------------- |
| Display segments a–g | PORTD (PD0–PD6)      |
| Display dp           | PD7                  |
| Common digit 1       | PB0 (via transistor) |
| Common digit 2       | PB1 (via transistor) |
| Common digit 3       | PB2 (via transistor) |
| Common digit 4       | PB3 (via transistor) |

### Wiring diagram (text)

```
              ATmega328P
            ┌───────────┐
   Seg a ───┤ PD0   PB0 ├──── COM1 ──[R]── Base NPN
   Seg b ───┤ PD1   PB1 ├──── COM2 ──[R]── Base NPN
   Seg c ───┤ PD2   PB2 ├──── COM3 ──[R]── Base NPN
   Seg d ───┤ PD3   PB3 ├──── COM4 ──[R]── Base NPN
   Seg e ───┤ PD4       │
   Seg f ───┤ PD5       │
   Seg g ───┤ PD6       │
   Seg dp ──┤ PD7       │
            └───────────┘
```

Each common pin drives an NPN transistor (2N3904, BC337, etc.) that
connects the common cathode of the corresponding digit to GND.

Segment bit mapping (PORTD, active low for common anode):

| Bit | 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Seg | a   | b   | c   | d   | e   | f   | g   | dp  |

## How it works

### Multiplexing

The 4 digits share the same 8 segment lines. Only one digit is
enabled at a time, cycling fast enough (~125 Hz) that the human eye
sees all 4 digits lit simultaneously.

Each digit is enabled for `TIME` ms (currently 2 ms). The full cycle
takes `4 × TIME = 8 ms`, giving a refresh rate of ~125 Hz.

### Timekeeping

Timer0 is configured in CTC mode with a prescaler of 64. At 8 MHz,
this generates an interrupt every 1 ms:

```
8,000,000 / 64 = 125,000 Hz  →  8 µs per tick
125 ticks × 8 µs = 1000 µs = 1 ms
OCR0A = 124 (counts 0..124 = 125 ticks)
```

Inside the ISR, a millisecond counter is incremented. When it reaches
1000, one second has passed:

- `seconds` increments every 1 s
- `minutes` increments every 60 s
- `hours` increments every 60 min
- `hours` wraps at 24

The main loop reads `hours` and `minutes`, splits them into 4 digits,
and calls `multiplexing()`.

## Digit table

The `seven_seg[]` array maps each decimal digit to the byte written
to PORTD. Values are **active low** (common anode):

```c
const uint8_t seven_seg[10] = {
    0b11000000,  // 0
    0b11111001,  // 1
    0b10100100,  // 2
    0b10110000,  // 3
    0b10011001,  // 4
    0b10010010,  // 5
    0b10000010,  // 6
    0b11111000,  // 7
    0b10000000,  // 8
    0b10010000,  // 9
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
framework
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
   (8 MHz). If it doesn't match, the Timer and `_delay_ms()` will
   not behave correctly.
3. Connect the 4-digit display segments to PORTD (with resistors)
   and the common pins to NPN transistors driven by PB0–PB3.
4. Load the `.hex` file into the MCU's program memory.
5. Run the simulation.

> Note: Proteus uses ideal switches with no mechanical bounce, and
> the simulated clock is perfectly accurate. The real hardware may
> drift slightly over time.

## File structure

```
4digit-7seg-multiplexing/
├── src/
│   └── main.c        # Everything: timer, multiplexing, clock logic
├── lib/              # PlatformIO private libraries
├── test/             # PlatformIO tests
├── platformio.ini
├── LICENSE
└── README.md
```

## Troubleshooting

| Symptom                       | Cause                           | Fix                                  |
| ----------------------------- | ------------------------------- | ------------------------------------ |
| Display shows nothing         | Wrong common logic              | Check NPN vs PNP, active low/high    |
| Only one digit lights up      | Multiplexing not cycling        | Verify PB0–PB3 toggling              |
| Display flickers              | `TIME` too high                 | Lower `TIME` to 1–2 ms               |
| Ghosting between digits       | No off-time between digits      | Add `_delay_us(100)` after disable   |
| Clock runs too fast/slow      | `F_CPU` mismatch                | Match `F_CPU` with Proteus clock     |
| Clock doesn't advance in 30 s | Normal — minutes change at 60 s | Wait 60 s or lower the threshold     |
| ISR never runs                | Missing `sei()`                 | Call `sei()` after configuring Timer |

## Future improvements

- Buttons to set hours and minutes
- Blinking colon (dp) at 1 Hz
- External RTC (DS3231) for accurate timekeeping
- 12/24-hour format switch

## License

MIT
