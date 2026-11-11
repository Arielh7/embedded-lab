# 🧪 LED Dimmer — ADC-Controlled PWM Brightness

![C](https://img.shields.io/badge/C-bare--metal-blue?logo=c)
![PlatformIO](https://img.shields.io/badge/PlatformIO-orange?logo=platformio)
![Proteus](https://img.shields.io/badge/Proteus-Simulation-green)
![ATmega328P](https://img.shields.io/badge/MCU-ATmega328P-red)

![Header](https://placehold.co/1200x300/1e1e1e/ffffff?text=ATmega328P+%2B+ADC+%2B+PWM+LED+Dimmer)

Bare-metal firmware that reads a **potentiometer through the ADC** and drives an LED with **hardware PWM**, so the brightness tracks the analog input in real time. The ADC runs in **free-running mode** with **interrupt-driven updates**, leaving the main loop free to refresh a **10-LED binary bar** showing the raw ADC value. Part of my embedded firmware collection built with **PlatformIO**, simulated in **Proteus** (no physical hardware).

---

## ✨ Features

- ✅ Bare-metal C (no Arduino framework)
- ✅ **ADC in free-running mode** — the hardware re-triggers itself, no polling in `main`
- ✅ **Interrupt-driven PWM update**: `OCR0A` refreshed inside `ISR(ADC_vect)`
- ✅ **Hardware PWM on Timer0** (Fast PWM, non-inverting, OC0A / PD6) at ~488 Hz
- ✅ **10-bit ADC mapped to a 10-LED binary bar** (8 on PORTB + 2 on PD0/PD1)
- ✅ Full 10-bit ADC range mapped to 8-bit duty (`adc >> 2`)
- ✅ `volatile`-correct shared state between ISR and `main`
- ✅ Internal 8 MHz RC oscillator — PB6/PB7 stay free as GPIO
- ✅ Target: ATmega328P @ 8 MHz

---

## 🛠️ Hardware (simulated in Proteus)

| Component              | Connection                             |
| ---------------------- | -------------------------------------- |
| Potentiometer (10 kΩ)  | Wiper → PC0 (ADC0), ends to +5 V / GND |
| PWM LED                | PD6 (OC0A) via 220–330 Ω               |
| ADC bits 7..0 (8 LEDs) | PB0–PB7 via 220–330 Ω                  |
| ADC bit 8              | PD0 via 220–330 Ω                      |
| ADC bit 9              | PD1 via 220–330 Ω                      |

> ⚠️ **Proteus simulation only.** Not tested on physical hardware.

### Why the internal oscillator

On the ATmega328P, **PB6/PB7 are shared with XTAL1/XTAL2**. Running from the **internal 8 MHz RC oscillator** frees them, so `DDRB = 0xFF` can drive all 8 bar LEDs. The trade-off: PWM drops from ~976 Hz to ~488 Hz and the ADC clock from 125 kHz to 62.5 kHz — both still within spec.

---

## 📁 Project structure

```
led-dimmer/
├── src/  (main.c — PWM init, ADC free-running init, ISR, LED bar)
├── platformio.ini
└── sim/  (led_dimmer.pdsprj)
```

---

## ⚙️ How it works

1.  **PWM setup (Timer0):** `TCCR0A = COM0A1 | WGM01 | WGM00` selects **Fast PWM** (8-bit, TOP = 0xFF) with **non-inverting** output on OC0A (PD6). `TCCR0B = CS01 | CS00` sets prescaler 64 → `f_PWM = 8 MHz / (64 × 256) ≈ 488 Hz`. Duty = `OCR0A / 256`; no timer interrupt needed — the waveform is pure hardware.
2.  **ADC free-running:** `ADMUX = REFS0` (AVcc reference, ADC0). `ADCSRB = 0` selects free-running auto-trigger. `ADCSRA = ADEN | ADATE | ADIE | ADPS2 | ADPS1 | ADPS0` enables the ADC, the auto-trigger, the conversion-complete interrupt, and prescaler 128 (ADC clock = 62.5 kHz, ~4800 samples/s). A single `ADSC` write starts the first conversion — the hardware keeps going from there.
3.  **ISR (`ADC_vect`):** reads the 10-bit result (`ADC` macro) into `adc_result`, then writes `OCR0A = adc_result >> 2` to map 0–1023 → 0–255. The LED follows the pot instantly, with no main-loop involvement.
4.  **Main loop:** refreshes the 10-LED binary bar. `PORTB = adc_result & 0xFF` shows the low 8 bits. PD0/PD1 show bits 8 and 9 via **masked writes** (`PORTD |= ...` / `PORTD &= ~...`) — **never** `PORTD = ...`, which would disconnect Timer0 from PD6 and kill the PWM.

### Design notes

- **No polling:** `main` never waits on `ADSC` — the ADC re-triggers itself.
- **No timer interrupt for PWM:** Timer0 generates the waveform entirely in hardware.
- **One owner per register:** PD6 belongs to the timer, so PORTD is only touched with bit masks.
- **10-bit resolution preserved:** the bar shows all 10 bits; the PWM uses the top 8.

---

## 📚 Key registers

| Register | Value                                              | Purpose                                  |
| -------- | -------------------------------------------------- | ---------------------------------------- |
| `ADMUX`  | `REFS0`                                            | AVcc reference, channel ADC0 (PC0)       |
| `ADCSRB` | `0`                                                | Auto-trigger source = free-running       |
| `ADCSRA` | `ADEN \| ADATE \| ADIE \| ADPS2 \| ADPS1 \| ADPS0` | ADC on, free-running, IRQ, prescaler 128 |
| `TCCR0A` | `COM0A1 \| WGM01 \| WGM00`                         | Fast PWM, non-inverting OC0A             |
| `TCCR0B` | `CS01 \| CS00`                                     | Prescaler 64 (~488 Hz)                   |
| `OCR0A`  | 0–255                                              | PWM duty                                 |

---

## 🚀 Build and simulate

```bash
pio run          # Generates the .hex
```

`platformio.ini`:

```ini
[env:ATmega328P]
platform = atmelavr
board = ATmega328P
board_build.f_cpu = 8000000L
```

1.  Open the `.pdsprj` file in Proteus.
2.  Double-click the MCU and set **Clock Frequency** to 8 MHz (must match `F_CPU`).
3.  Load the `.hex` generated by PlatformIO into program memory.
4.  Run the simulation — turn the pot and watch the binary bar update; put the **virtual oscilloscope on PD6** to confirm the duty cycle tracks the input.

---

## 📝 Notes & limitations

- **Proteus doesn't render PWM brightness realistically** — the LED may look dim or inconsistent. The oscilloscope on PD6 is the reliable way to verify the duty cycle.
- **Never write `PORTD` as a whole** in this project: PD6 is owned by the timer. Always use masked writes for PD0/PD1.
- **`adc_result` must be `volatile`** — it's written in the ISR and read in `main`.
- **Free-running ADC runs continuously** even when nobody needs the value. Fine here, but a single-shot or triggered mode would save power in a battery design.
- **Internal RC oscillator** is less accurate than a crystal (±10 % over temperature). Fine for an LED dimmer; not fine for timing-critical work.
- Troubleshooting quick-ref:
  - LED doesn't dim → check PD6 is not being overwritten by a whole-port write.
  - Bar doesn't move → verify ADC0 wiring and that `ADSC` was written once.
  - PWM looks wrong on scope → confirm `F_CPU` = 8 MHz matches the Proteus clock.
  - Only low 8 LEDs work → check PD0/PD1 masked writes for bits 8 and 9.
- **Future improvements:** 16-bit Timer1 for higher PWM resolution, gamma correction for perceptual brightness, multiple PWM channels (RGB dimmer), LCD readout of the raw ADC value.
