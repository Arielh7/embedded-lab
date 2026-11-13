# 🧪 UART-LCD — USART Between Two ATmega328P with LCD

![C](https://img.shields.io/badge/C-bare--metal-blue?logo=c)
![PlatformIO](https://img.shields.io/badge/PlatformIO-orange?logo=platformio)
![Proteus](https://img.shields.io/badge/Proteus-Simulation-green)
![ATmega328P](https://img.shields.io/badge/MCU-ATmega328P-red)

![Header](https://placehold.co/1200x300/1e1e1e/ffffff?text=ATmega328P+%2B+USART+%2B+LCD)

Bare-metal firmware demonstrating **asynchronous serial communication (USART)** between **two ATmega328P** chips: one transmits, the other receives and displays each line on a **16×2 HD44780 LCD**. The receiver uses **interrupt-driven RX** with a **circular buffer**, so the main loop never blocks on incoming bytes. Part of my embedded firmware collection built with **PlatformIO**, simulated in **Proteus** (no physical hardware).

---

## ✨ Features

- ✅ Bare-metal C (no Arduino framework)
- ✅ **USART at 9600 baud** (8N1), 16 MHz clock → `UBRR = 103`
- ✅ **Interrupt-driven reception** (`USART_RX_vect`) + **circular buffer**
- ✅ **Line detection** on `\r` or `\n`, then print to LCD
- ✅ Reusable **HD44780 LCD driver** in 4-bit mode on PD2–PD7
- ✅ `volatile`-correct shared state between ISR and `main`
- ✅ Two independent PlatformIO projects: `transmisor/` and `receptor/`
- ✅ Target: ATmega328P @ 16 MHz

---

## 🛠️ Hardware (simulated in Proteus)

### MCU-to-MCU

| Transmitter | Receiver  |
| ----------- | --------- |
| PD1 (TXD)   | PD0 (RXD) |
| PD0 (RXD)   | PD1 (TXD) |
| GND         | GND       |

### LCD on the receiver

| LCD signal | ATmega328P pin                  |
| ---------- | ------------------------------- |
| RS         | PD2                             |
| E          | PD3                             |
| D4         | PD4                             |
| D5         | PD5                             |
| D6         | PD6                             |
| D7         | PD7                             |
| VSS, RW    | GND                             |
| VDD        | 5 V                             |
| V0         | Potentiometer to GND (contrast) |

> ⚠️ **PD0 and PD1 are reserved for USART.** The LCD driver must never touch them.

> ⚠️ **Proteus simulation only.** Not tested on physical hardware.

---

## 📁 Project structure

```
uart-lcd/
├── transmisor/
│   ├── platformio.ini
│   └── src/  (main.c — TX loop)
└── receptor/
    ├── platformio.ini
    ├── lib/lcd/  (lcd.c, lcd.h)
    └── src/  (main.c — RX ISR, buffer, LCD)
```

---

## ⚙️ How it works

1.  **Transmitter:** configures USART at 9600 baud (16 MHz → `UBRR0 = 103`), 8N1. Every second it sends `"Hola mundo\r\n"` by polling `UDRE0` before each `UDR0` write.
2.  **Receiver — USART setup:** same baud rate and frame format, plus `RXCIE0` to enable the **receive-complete interrupt**.
3.  **ISR (`USART_RX_vect`):** reads `UDR0` and pushes the byte into a **circular buffer** (64 bytes), advancing `head`. If the buffer is full (63 useful bytes), the new byte is silently dropped.
4.  **Main loop:** calls `uart_leer()`, which pops bytes from the buffer (advancing `tail`) and appends them to a line buffer. When `\r` or `\n` arrives, the string is null-terminated, the LCD is cleared, and the line is printed.
5.  **LCD driver:** HD44780 in 4-bit mode on PD2–PD7. `lcd_puts()` is the actual print function exposed by the driver.

### Circular buffer

```c
#define BUF_SIZE 64
volatile char buffer[BUF_SIZE];
volatile uint8_t head = 0;   // written by ISR
volatile uint8_t tail = 0;   // read by main
```

With one line per second at 9600 baud, the buffer never fills — but it would absorb bursts if the main loop were busy.

### Line detection

`\r\n` produces two bytes. The first closes and prints the line; the second arrives with an empty `idx` and produces no output. This avoids double-printing.

---

## 📚 Key registers

| Register   | Value                      | Purpose                               |
| ---------- | -------------------------- | ------------------------------------- |
| `UBRR0H/L` | `103`                      | 9600 baud @ 16 MHz (U2X = 0)          |
| `UCSR0B`   | `TXEN0 \| RXEN0 \| RXCIE0` | Enable TX, RX, and RX interrupt       |
| `UCSR0C`   | `UCSZ01 \| UCSZ00`         | 8 data bits, no parity, 1 stop        |
| `UCSR0A`   | poll `UDRE0` / `RXC0`      | TX-ready / RX-ready flags             |
| `UDR0`     | —                          | Data register (write = TX, read = RX) |

---

## 🚀 Build and simulate

```bash
cd transmisor && pio run
cd ../receptor && pio run
```

Each project's `platformio.ini`:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
```

The `.hex` files land in `.pio/build/uno/firmware.hex`.

> `F_CPU` is defined in the C source (`#define F_CPU 16000000UL` before `<util/delay.h>`). Don't put it loose in `platformio.ini` — it causes a compile error.

1.  Open the `.pdsprj` file in Proteus.
2.  Place **two** ATmega328P chips.
3.  On **both**, set `Clock Frequency = 16000000` (must match `F_CPU`, or the link fails).
4.  Load each chip's `.hex` into its `Program File` property.
5.  Wire TX↔RX crossed, GND common, and the LCD per the tables above.
6.  Run the simulation — the LCD should show `Hola mundo`, refreshed each second.

---

## 📝 Notes & limitations

- **`Clock Frequency` in Proteus must equal `F_CPU`.** This is the #1 cause of garbled characters or a dead link. Both are 16 MHz here.
- **TX/RX must be crossed** (TX→RX, RX→TX), and **GND must be common** between the two MCUs. Missing either produces silence.
- **PD0/PD1 belong to USART** — the LCD driver uses PD2–PD7 only. Accidentally writing `PORTD` as a whole would break the link.
- **The LCD print function is `lcd_puts()`**, not `lcd_print()`. Verify the driver API before calling.
- **Buffer overflow is silent** — with the current workload it never happens, but a faster sender would need flow control or a bigger buffer.
- **Proteus has no serial noise** — on real hardware, mismatched baud rates, long cables, or missing GND would corrupt frames. The firmware doesn't implement parity or checksums.
- Troubleshooting quick-ref:
  - Blank LCD → adjust V0 contrast (pot to GND).
  - Garbage characters → `F_CPU` ≠ Proteus clock, or baud mismatch.
  - Nothing arrives → TX/RX not crossed, or GND not shared.
  - `F_CPU` compile error → remove it from `platformio.ini`, keep it in C only.
  - LCD shows nothing despite RX → check `lcd_puts()` exists in the driver.
- **Future improvements:** command protocol (e.g. `SET`/`GET`), echo/ACK from the receiver, hardware flow control (RTS/CTS), ring buffer statistics on the LCD, 115200 baud with `U2X0`.
