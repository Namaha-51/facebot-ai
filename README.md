# FaceBot AI — Embedded System & Real-Time Serial Companion

![Build Status](https://img.shields.io/badge/Platform-ESP32--WROOM--32-blue?style=flat-square&logo=expressif)
![Language](https://img.shields.io/badge/Language-C++17%20%7C%20Python%203.11-green?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-Event--Driven%20%7C%20Non--Blocking-orange?style=flat-square)

An event-driven, micro-sized desktop companion combining an ESP32 microcontroller with a host-side Python runtime for real-time LLM inference, dynamic peripheral actuation, and serial state machine coordination.

---

## 🔬 Hardware & Peripheral Architecture

The target hardware operates on a 32-bit Xtensa dual-core ESP32 microcontroller running custom C++ firmware. Peripherals are isolated by bus type to minimize line contention and maximize dynamic response latency.

                  +--------------------------+
                  |    Host Machine (PC)     |
                  |  Python Host Runtime /   |
                  |   Gemini 2.5 API Bridge  |
                  +------------+-------------+
                               |
                   UART / USB  | 115200 Baud
                   Raw Strings | (8N1)
                               v
                  +------------+-------------+
                  |  ESP32 Microcontroller   |
                  |  (Non-blocking Timers)   |
                  +----+-------+-------+-----+
                       |       |       |
      +----------------+       |       +----------------+
      | SPI Bus                | PWM                    | GPIO Direct
      v                        v                        v
+-----+----------+      +------+-------+        +-------+------+
| MAX7219 8x8    |      | SG90 Servo   |        | HD44780 16x2 |
| LED Matrix     |      | Motor        |        | Parallel LCD |
+----------------+      +--------------+        +--------------+

---

## ⚡ Embedded Design Highlights

* **Deterministic State Machine:** Built entirely on non-blocking millis() timing loops to eliminate CPU stall states during UART parsing, animation updates, or peripheral toggles.
* **Low-Power State Transitions:** Implements dynamic hardware power-down modes. Upon entering SLEEP state, timer interrupts detach the PWM servo driver (eliminating holding torque jitter) and disable the LCD backlight driver.
* **Stream-Based Serial Parser:** Delimits incoming UART frames over USB at 115200 Baud using a custom [COMMAND]:[PAYLOAD] protocol parser with automated buffer clearing.
* **Synchronized Actuation Dynamics:** Interleaved execution pipelines sync character-by-character LCD text rendering with 8x8 LED matrix frame shifting, servo angle manipulation, and frequency-modulated active buzzer feedback.

---

## 📍 System Pin Mapping & Interface Matrix

| Peripheral Module | Physical Interface | ESP32 GPIO | Operating Parameters |
| :--- | :--- | :--- | :--- |
| **MAX7219 Matrix CS** | SPI (Software/Hardware) | GPIO 5 | Active LOW |
| **MAX7219 Matrix CLK** | SPI Clock | GPIO 18 | Hardware SPI Bus |
| **MAX7219 Matrix DIN** | SPI MOSI | GPIO 23 | Bit-banged / Hardware MOSI |
| **SG90 Servo Motor** | PWM Signal | GPIO 32 | 50 Hz, 0.5ms–2.5ms Pulse Width |
| **Active Buzzer** | Direct Digital / Tone | GPIO 33 | Frequency Sweep Modulation |
| **HD44780 LCD RS** | Parallel Control | GPIO 13 | Register Select |
| **HD44780 LCD Enable** | Parallel Control | GPIO 12 | Strobe Bit |
| **HD44780 LCD Data** | 4-Bit Parallel Bus | GPIO 14, 27, 26, 25 | Nibble Mode Bus Integration |

---

## ⏱️ Firmware State Machine Lifecycle

 +-------------------------------------------------------------------+
 |                                                                   |
 v                                                                   |
[STATE 0: ACTIVE] --(15s Silence)--> [STATE 1: RESTLESS]             |
        |                                   |                        |
        | (Incoming UART Payload)           | (45s Silence)          |
        |                                   v                        |
        +---------------------------- [STATE 2: BORED]               |
        |                                   |                        |
        |                                   | (90s Silence)          |
        |                                   v                        |
        +---------------------------- [STATE 3: SLEEP]---------------+
                                       - Detach Servo PWM
                                       - Disable LCD Drivers

---

## 🚀 Installation & Build Pipeline

### Firmware Compiling (ESP32)
1. Open the project root in VS Code with PlatformIO or Arduino IDE extensions configured for ESP32.
2. Ensure required C++ driver libraries are installed:
   * MD_MAX72xx (LED Matrix Driver)
   * LiquidCrystal (Parallel LCD Driver)
   * ESP32Servo (PWM Actuation Driver)
3. Flash firmware/src/main.cpp via target USB/UART port (115200 Baud).

### Python Host Runtime
1. Navigate to host directory and install dependencies:
   cd python_host
   pip install -r requirements.txt
2. Configure environment credentials:
   export GEMINI_API_KEY="your-api-key"
3. Run communication bridge:
   python main.py

---

## 📂 Repository Structure

facebot-ai/
├── firmware/
│   └── src/
│       └── main.cpp          # Non-blocking C++ State Machine & Peripheral Drivers
├── python_host/
│   ├── main.py               # Serial protocol encoding & Gemini LLM Runtime
│   └── requirements.txt      # Python runtime dependencies
├── docs/
│   └── hardware_setup.md     # Wiring schematic & interface specifications
├── .gitignore                # Target build & key isolation
└── README.md                 # System Architecture Datasheet