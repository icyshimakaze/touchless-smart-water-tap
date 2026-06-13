# 🚰 Smart Touchless Water Tap Opener

> A contactless, Arduino-powered water tap controller that opens and closes automatically using ultrasonic hand detection — with smooth servo motion, noise filtering, auto-close timer, water-usage monitoring, OLED display, and leak detection.

---

## 📌 Table of Contents

1. [Project Overview](#project-overview)  
2. [Demo](#demo)  
3. [Circuit Diagram](#circuit-diagram)  
4. [Components List](#components-list)  
5. [Working Principle](#working-principle)  
6. [Features](#features)  
7. [Getting Started](#getting-started)  
8. [Configuration](#configuration)  
9. [Future Improvements](#future-improvements)  
10. [License](#license)

---

## Project Overview

Traditional taps require physical contact — a hygiene problem in kitchens, labs, hospitals, and public spaces. This project converts any manual tap into a **touchless smart tap** using an ultrasonic sensor and a servo motor, all controlled by an Arduino Uno.

A recruiter or judge can understand the complete project from this README alone — no code reading required.

---

## Demo

> 📸 *Replace the placeholders below with your actual photos/GIF*

| Prototype (Side View) | OLED in Action | Auto-Close Demo |
|---|---|---|
| `[photo_prototype.jpg]` | `[photo_oled.jpg]` | `[gif_autoclose.gif]` |

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

> 🖼️ A visual Fritzing diagram is available in [`/circuit/circuit_diagram.svg`](./circuit/circuit_diagram.svg)

---

## Components List

| # | Component | Specification | Quantity | Est. Cost (INR) |
|---|-----------|---------------|----------|-----------------|
| 1 | Arduino Uno | ATmega328P | 1 | ₹400 |
| 2 | Ultrasonic Sensor | HC-SR04 (2 cm – 400 cm) | 1 | ₹80 |
| 3 | Servo Motor | SG90 / MG996R (for real taps) | 1 | ₹120 – ₹350 |
| 4 | OLED Display | 0.91" I2C, 128×32, SSD1306 | 1 | ₹180 |
| 5 | Buzzer | Passive, 5V | 1 | ₹20 |
| 6 | Jumper Wires | Male–Male / Male–Female | ~20 | ₹50 |
| 7 | Breadboard | 830-tie point | 1 | ₹80 |
| 8 | USB Cable | Type A to Type B | 1 | ₹60 |
| **Total** | | | | **≈ ₹990 – ₹1,200** |

> 💡 *For a real tap installation, replace SG90 with a high-torque MG996R and mount using a 3D-printed bracket.*

---

## Working Principle

```
 ┌─────────────┐     ┌──────────────┐     ┌──────────────────┐
 │  HC-SR04    │────▶│  Arduino Uno │────▶│   Servo Motor    │
 │ (distance)  │     │  (logic)     │     │ (open/close tap) │
 └─────────────┘     └──────┬───────┘     └──────────────────┘
                            │
                     ┌──────▼───────┐
                     │ OLED Display │
                     │ + Buzzer     │
                     └──────────────┘
```

### Step-by-Step Flow

1. **Sense** — The HC-SR04 fires a 40 kHz ultrasonic pulse every 100 ms and measures the echo return time.

2. **Filter** — 5 consecutive readings are taken and averaged (after dropping the outlier min/max) to eliminate noise.

3. **Decide** — If the smoothed distance is ≤ 4.0 cm (configurable), a hand is detected.

4. **Open** — The servo sweeps smoothly from 0° → 180° in 1° increments to avoid sudden water surges.

5. **Auto-Close** — If no hand is detected for 5 seconds, the servo sweeps back to 0°.

6. **Monitor** — Cumulative tap-open time is tracked and shown on the OLED as a water-usage indicator.

7. **Leak Alert** — If the sensor reads < 1.0 cm while the tap is closed, the buzzer sounds a 3-beep alert.

---

## Features

| Feature | v1 (Original) | v2 (This Version) |
|---|---|---|
| Hand detection | ✅ Basic | ✅ Improved |
| Servo control | ⚠️ Abrupt jump | ✅ Smooth 1°/step sweep |
| Noise filtering | ❌ None | ✅ 5-sample averaged |
| Configurable threshold | ❌ Hardcoded `4.0` | ✅ `THRESHOLD_DISTANCE` constant |
| Auto-close timer | ❌ | ✅ 5 s inactivity timeout |
| Water usage monitor | ❌ | ✅ Session open-time in seconds |
| OLED display | ❌ | ✅ Distance + Status + Usage |
| Leak detection | ❌ | ✅ Buzzer alert |
| Serial debug output | ⚠️ Basic | ✅ Detailed |

---

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
- Libraries (install via Library Manager):
  - `Adafruit GFX Library`
  - `Adafruit SSD1306`

### Upload Steps

```bash
1. Clone this repository
   git clone https://github.com/<your-username>/smart-water-tap.git

2. Open the sketch
   Arduino IDE → File → Open → watertap_opener.ino

3. Install libraries
   Tools → Manage Libraries → Search and install:
     - Adafruit GFX Library
     - Adafruit SSD1306

4. Select board & port
   Tools → Board → Arduino Uno
   Tools → Port → (your COM port)

5. Upload
   Click the Upload (→) button
```

> ⚠️ *If you don't have an OLED, the code still runs — it prints a warning to Serial and continues.*

---

## Configuration

All tunable values are grouped at the top of the `.ino` file:

```cpp
const float THRESHOLD_DISTANCE  = 4.0;    // Hand detection range in cm
const int   SAMPLES             = 5;      // Readings averaged per cycle
const int   OPEN_ANGLE          = 180;    // Servo angle for OPEN
const int   CLOSED_ANGLE        = 0;      // Servo angle for CLOSED
const int   SWEEP_STEP_DELAY_MS = 15;     // Smoothness of sweep (lower = faster)
const unsigned long AUTO_CLOSE_DELAY_MS = 5000; // Auto-close wait time in ms
const float LEAK_THRESHOLD_CM   = 1.0;   // Leak alert trigger distance
```

No need to hunt through the code — change these values and re-upload.

---

## Future Improvements

| Priority | Feature | Description |
|---|---|---|
| 🔴 High | **ESP32 migration** | Replace Arduino Uno with ESP32 for built-in Wi-Fi/BLE |
| 🔴 High | **IoT dashboard** | Push usage data to ThingSpeak / Firebase in real time |
| 🟡 Medium | **Mobile app** | Flutter app to view daily usage and set thresholds remotely |
| 🟡 Medium | **Flow sensor** | Replace time-based usage with actual litre measurement (YF-S201 sensor) |
| 🟡 Medium | **3D-printed mount** | Custom bracket to fit standard Indian tap knobs |
| 🟢 Low | **OTA updates** | Over-the-air firmware update via ESP32 |
| 🟢 Low | **Multi-user profiles** | Detect presence duration for child vs adult tap timing |
| 🟢 Low | **Battery backup** | 18650 LiPo + TP4056 charger for power-cut resilience |

---

## Project Structure

```
smart-water-tap/
├── watertap_opener.ino      # Main Arduino sketch (v2.0)
├── circuit/
│   └── circuit_diagram.svg  # Visual wiring diagram
├── images/
│   ├── prototype.jpg        # Add your photos here
│   └── oled_demo.jpg
├── README.md
└── LICENSE
```

---

## License

This project is open source under the [MIT License](LICENSE). Feel free to fork, modify, and build on it!

---

*Built with ❤️ using Arduino Uno · HC-SR04 · SG90 · SSD1306 OLED*
