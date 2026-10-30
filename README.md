# 🧪 Embedded Lab

![C](https://img.shields.io/badge/C-bare--metal-blue?logo=c)
![PlatformIO](https://img.shields.io/badge/PlatformIO-orange?logo=platformio)
![Proteus](https://img.shields.io/badge/Proteus-Simulation-green)
![ATmega328P](https://img.shields.io/badge/MCU-ATmega328P-red)

![Header](https://placehold.co/1200x300/1e1e1e/ffffff?text=Embedded+Lab+%E2%80%94+Firmware+%26+Proteus+Simulation)

My personal embedded lab: a growing collection of **bare-metal C firmware projects** for microcontrollers, **all simulated in Proteus** — no physical hardware required. Every project is built from scratch at the register level, without the Arduino framework.

---

## Purpose

- ✅ Learn firmware development from scratch
- ✅ Keep a consistent project structure across different MCUs
- ✅ Document every project for future reference
- ✅ Build a reusable set of drivers and patterns

---

## Tooling

| Tool           | Role                                           |
| -------------- | ---------------------------------------------- |
| **PlatformIO** | Build system and toolchain (VS Code extension) |
| **Proteus**    | Circuit simulation                             |
| **Git**        | Version control                                |

---

## Working with a project

```bash
cd <project-folder>
pio run          # generate the .hex
```

Then open the `.pdsprj` file in Proteus, set the MCU clock to match `board_build.f_cpu`, and load the generated `.hex` into program memory.

---
