// ============================================================
//  Smart Touchless Water Tap Opener
//  Author  : [Your Name]
//  Board   : Arduino Uno
//  Version : 2.0
//  Date    : June 2025
// ============================================================
//
//  HARDWARE CONNECTIONS
//  --------------------
//  HC-SR04 Trig  → Pin 10
//  HC-SR04 Echo  → Pin 11
//  Servo Signal  → Pin 6
//  OLED SDA      → A4  (I2C)
//  OLED SCL      → A5  (I2C)
//  Buzzer        → Pin 8  (optional feedback)
//
// ============================================================

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ── Pin Definitions ─────────────────────────────────────────
const int SERVO_PIN  = 6;
const int TRIG_PIN   = 10;
const int ECHO_PIN   = 11;
const int BUZZER_PIN = 8;

// ── Tunable Parameters ───────────────────────────────────────
const float THRESHOLD_DISTANCE  = 4.0;   // cm – hand detection range
const int   SAMPLES             = 5;     // readings averaged per cycle
const int   OPEN_ANGLE          = 180;   // servo angle for OPEN position
const int   CLOSED_ANGLE        = 0;     // servo angle for CLOSED position
const int   SWEEP_STEP_DELAY_MS = 15;    // ms between each 1° servo step
const unsigned long AUTO_CLOSE_DELAY_MS = 5000; // ms before auto-close (5 s)
const float LEAK_THRESHOLD_CM   = 1.0;  // reading below this = possible leak

// ── OLED Setup ───────────────────────────────────────────────
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── Runtime State ────────────────────────────────────────────
Servo servo1;
bool  tapOpen               = false;
unsigned long lastDetectedMs = 0;   // timestamp of last hand detection
unsigned long totalOpenMs    = 0;   // cumulative time tap was open (for usage)
unsigned long openStartMs    = 0;   // when the current open session started
int  currentAngle           = CLOSED_ANGLE;

// ── Helper: Read smoothed distance ──────────────────────────
// Takes SAMPLES ultrasonic readings, discards the outlier min/max,
// and returns the average to reduce sensor noise.
float getSmoothedDistance() {
  float readings[SAMPLES];

  for (int i = 0; i < SAMPLES; i++) {
    // Send a 10 µs trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure echo duration and convert to cm
    float duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout
    readings[i]    = (0.0343 * duration) / 2.0;
    delay(20); // short gap between pings to avoid echo interference
  }

  // Sort readings (simple bubble sort on SAMPLES small array)
  for (int i = 0; i < SAMPLES - 1; i++) {
    for (int j = 0; j < SAMPLES - 1 - i; j++) {
      if (readings[j] > readings[j + 1]) {
        float tmp      = readings[j];
        readings[j]    = readings[j + 1];
        readings[j + 1] = tmp;
      }
    }
  }

  // Average the middle values (drop lowest and highest)
  float sum = 0;
  for (int i = 1; i < SAMPLES - 1; i++) sum += readings[i];
  return sum / (SAMPLES - 2);
}

// ── Helper: Smooth servo sweep ───────────────────────────────
// Moves servo one degree at a time to avoid jerky water flow.
void sweepServo(int fromAngle, int toAngle) {
  if (fromAngle == toAngle) return;
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int a = fromAngle; a != toAngle; a += step) {
    servo1.write(a);
    delay(SWEEP_STEP_DELAY_MS);
  }
  servo1.write(toAngle);
  currentAngle = toAngle;
}

// ── Helper: Update OLED display ─────────────────────────────
void updateDisplay(float dist, bool open, unsigned long usageSec) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Row 1 – status
  display.setCursor(0, 0);
  display.print("Status: ");
  display.print(open ? "OPEN  " : "CLOSED");

  // Row 2 – distance
  display.setCursor(0, 10);
  display.print("Dist : ");
  display.print(dist, 1);
  display.print(" cm");

  // Row 3 – cumulative usage
  display.setCursor(0, 20);
  display.print("Usage: ");
  display.print(usageSec);
  display.print(" s");

  display.display();
}

// ── Helper: Leak detection ───────────────────────────────────
// If the sensor reads an abnormally short distance while tap is
// supposed to be closed, alert via buzzer (possible standing water).
void checkForLeak(float dist) {
  if (!tapOpen && dist < LEAK_THRESHOLD_CM) {
    Serial.println("[ALERT] Possible leak detected!");
    // Three short beeps
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 1000, 200);
      delay(400);
    }
  }
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("=== Smart Touchless Tap v2.0 ===");

  pinMode(TRIG_PIN,  OUTPUT);
  pinMode(ECHO_PIN,  INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  servo1.attach(SERVO_PIN);
  servo1.write(CLOSED_ANGLE);   // ensure tap starts closed
  currentAngle = CLOSED_ANGLE;

  // Initialise OLED (address 0x3C is standard for most modules)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[WARN] OLED not found – continuing without display.");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("Tap Ready!");
    display.display();
    delay(1500);
  }
}

// ── Main Loop ────────────────────────────────────────────────
void loop() {
  float dist = getSmoothedDistance();

  // Accumulate open time for water-usage monitoring
  if (tapOpen) {
    totalOpenMs = (millis() - openStartMs);
  }

  Serial.print("Distance: ");
  Serial.print(dist, 2);
  Serial.print(" cm | Tap: ");
  Serial.println(tapOpen ? "OPEN" : "CLOSED");

  // ── Hand detected ──
  if (dist <= THRESHOLD_DISTANCE && dist > LEAK_THRESHOLD_CM) {
    lastDetectedMs = millis();  // reset auto-close timer

    if (!tapOpen) {
      Serial.println(">> Hand detected – opening tap");
      tone(BUZZER_PIN, 1500, 100); // brief beep on open
      sweepServo(currentAngle, OPEN_ANGLE);
      tapOpen      = true;
      openStartMs  = millis();
    }
  }

  // ── Auto-close after inactivity ──
  if (tapOpen && (millis() - lastDetectedMs >= AUTO_CLOSE_DELAY_MS)) {
    Serial.println(">> No hand – auto-closing tap");
    sweepServo(currentAngle, CLOSED_ANGLE);
    tapOpen = false;

    Serial.print("Session water-on time: ");
    Serial.print(totalOpenMs / 1000);
    Serial.println(" s");
    totalOpenMs = 0;
  }

  // ── Leak detection ──
  checkForLeak(dist);

  // ── OLED refresh ──
  updateDisplay(dist, tapOpen, totalOpenMs / 1000);

  delay(100); // loop cadence
}
