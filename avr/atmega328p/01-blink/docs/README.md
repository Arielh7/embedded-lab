# 01-blink

LED blink on PD0 using an ATmega328P.

## Goal

Learn the basic workflow of:

- Configuring a pin as output (DDRx).
- Turning a pin on/off (PORTx).
- Generating delays with `_delay_ms`.
- Simulating in Proteus without physical hardware.

## Hardware

| Item       | Detail                                     |
| ---------- | ------------------------------------------ |
| MCU        | ATmega328P                                 |
| Clock      | 8000000 Hz (see `platformio.ini`)          |
| Pin used   | PD0 (pin 2 on DIP package)                 |
| Components | 1 LED, 1 resistor 220Ω                     |
| Reset      | 10kΩ pull-up between RESET (pin 1) and +5V |

## Behavior

The LED toggles on/off every 1000 ms.
Full cycle: 2 second.

## How to build

```bash
pio run
```
