# 🚰 Smart Touchless Water Tap Opener

> A contactless, Arduino-powered water tap that opens and closes automatically using ultrasonic hand detection — with smooth servo motion, noise filtering, auto-close timer, water-usage monitoring, OLED display, and leak detection.

---

## Project Overview

Traditional taps require physical contact — a hygiene problem in kitchens, labs, hospitals, and public spaces. This project converts any manual tap into a **touchless smart tap** using an ultrasonic sensor and a servo motor, controlled by an Arduino Uno.

---

## Circuit Diagram

```
                          Arduino Uno
                       ┌─────────────────┐
              5V ──────┤ 5V         GND  ├──── GND (common)
             GND ──────┤ GND             │
                       │                 │
    HC-SR04 Trig ──────┤ Pin 10          │
    HC-SR04 Echo ──────┤ Pin 11          │
                       │                 │
      Servo Signal ────┤ Pin 6   (PWM)   │
      Servo VCC ───────┤ 5V              │
      Servo GND ───────┤ GND             │
                       │                 │
      OLED SDA ─────── ┤ A4  (I2C SDA)  │
      OLED SCL ─────── ┤ A5  (I2C SCL)  │
                       │                 │
      Buzzer (+) ──────┤ Pin 8           │
      Buzzer (-) ──────┤ GND             │
                       └─────────────────┘
```

Visual diagram: [`/circuit/circuit_diagram.svg`](./circuit/circuit_diagram.svg)

---

## Components List

| # | Component | Specification | Qty | Est. Cost (INR) |
|---|-----------|---------------|-----|-----------------|
| 1 | Arduino Uno | ATmega328P | 1 | ₹400 |
| 2 | Ultrasonic Sensor | HC-SR04 | 1 | ₹80 |
| 3 | Servo Motor | SG90 / MG996R | 1 | ₹120 – ₹350 |
| 4 | OLED Display | 0.91" I2C SSD1306 | 1 | ₹180 |
| 5 | Buzzer | Passive, 5V | 1 | ₹20 |
| 6 | Jumper Wires | M-M / M-F | ~20 | ₹50 |
| 7 | Breadboard | 830-tie point | 1 | ₹80 |
| 8 | USB Cable | Type A to Type B | 1 | ₹60 |
| **Total** | | | | **≈ ₹990 – ₹1,200** |

---

## Working Principle

1. **Sense** — HC-SR04 fires an ultrasonic pulse and measures echo return time.
2. **Filter** — 5 readings are averaged (outliers dropped) to eliminate noise.
3. **Decide** — If distance ≤ 4.0 cm, a hand is detected.
4. **Open** — Servo sweeps smoothly from 0° → 180° in 1° steps.
5. **Auto-Close** — Tap closes automatically after 5 seconds of no hand.
6. **Monitor** — Open time is tracked and shown on the OLED display.
7. **Leak Alert** — If sensor reads < 1.0 cm while tap is closed, buzzer fires.

---

## Features

| Feature | v1 | v2 |
|---|---|---|
| Hand detection | ✅ Basic | ✅ Improved |
| Servo control | ⚠️ Abrupt jump | ✅ Smooth sweep |
| Noise filtering | ❌ | ✅ 5-sample average |
| Configurable threshold | ❌ Hardcoded | ✅ Named constant |
| Auto-close timer | ❌ | ✅ 5 s timeout |
| Water usage monitor | ❌ | ✅ Session time tracking |
| OLED display | ❌ | ✅ Status + distance + usage |
| Leak detection | ❌ | ✅ Buzzer alert |

---

## Setup

**Libraries required** (install via Arduino Library Manager):
- `Adafruit GFX Library`
- `Adafruit SSD1306`

Open `watertap_opener.ino` in Arduino IDE, select **Board: Arduino Uno**, choose your port, and upload.

---

## Configuration

All tunable values are at the top of the sketch:

```cpp
const float THRESHOLD_DISTANCE  = 4.0;   // Hand detection range (cm)
const int   SAMPLES             = 5;     // Readings averaged per cycle
const int   OPEN_ANGLE          = 180;   // Servo angle for OPEN
const int   CLOSED_ANGLE        = 0;     // Servo angle for CLOSED
const int   SWEEP_STEP_DELAY_MS = 15;    // Sweep speed (lower = faster)
const unsigned long AUTO_CLOSE_DELAY_MS = 5000; // Auto-close delay (ms)
const float LEAK_THRESHOLD_CM   = 1.0;  // Leak alert trigger (cm)
```

---

## Future Improvements

- **ESP32 migration** — Wi-Fi/BLE for IoT dashboard (ThingSpeak / Firebase)
- **Mobile app** — View usage and set thresholds remotely
- **Flow sensor** — Measure actual litres instead of time-based tracking
- **3D-printed mount** — Custom bracket for standard tap knobs
- **OTA updates** — Over-the-air firmware via ESP32
- **Battery backup** — 18650 LiPo + TP4056 for power-cut resilience

---

*Built with Arduino Uno · HC-SR04 · SG90 · SSD1306 OLED*
