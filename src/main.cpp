/*
 * Session 2 — read the MPU-6050 and print CSV over serial at ~50 Hz.
 * --------------------------------------------------------------------------
 * Copy this into src/main.cpp. Sanity-check against physics you know:
 *   flat on the table -> Z is about +9.8 (that's gravity!), X and Y near 0.
 *
 * Record a dataset (one file per gesture) from your laptop:
 *   python tools/record_csv.py wave_01.csv
 * Close Serial Monitor first. Wait for GO; move until STOP (10 seconds).
 * Use PlatformIO's Python; see README for Windows/Mac commands.
 * ...then upload the CSVs to Edge Impulse next session.
 * 
 * 
 * * HOMEWORK — tilt switches, knob dims:
 *   Combine this example with examples/session02_mpu_read.cpp in src/main.cpp.
 *   Keep one setup() and one loop(), and keep your working platformio.ini.
 *   MPU wiring: 3V3, GND, SDA GPIO8, SCL GPIO9. Keep the LED and pot above.
 *
 *   1. Choose one accelerometer axis. With its positive direction pointing up,
 *      enable the LED; held sideways or down, turn the LED off.
 *      Read the acceleration in those poses and choose a threshold between
 *      them. Judge orientation while held still, not during a shake.
 *   2. While enabled, the pot sets brightness. While disabled, the LED stays
 *      off at every knob setting. The existing raw / 16 mapping is fine.
 *   3. Test up -> sideways -> down -> up, plus two knob settings while enabled.
 *      Use a nonzero knob setting when checking the orientation switch.
 *   4. Add three comment lines at the top of src/main.cpp:
 *      - My chosen axis and threshold.
 *      - The readings I observed in the three poses.
 *      - One thing that surprised me when testing.
 *   5. Commit and push as FINAL: tilt switches, knob dims.
 *      Submit that commit's link in Moodle with your AI-use line.
 *
 *   Both controls must work; a brightness curve or perfectly flicker-free
 *   switching is not required. Optional: reduce flicker near the threshold.
 *   The unchanged knob-only example below is the class starting point.
 */

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

#define POT_PIN 4
#define LED_PIN 40

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(8, 9);                   // SDA 8, SCL 9

  if (!mpu.begin()) {
    Serial.println("MPU-6050 not found — run the I2C scanner first.");
    while (true) delay(1000);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
}

void loop() {
  //POT READING
  int raw  = analogRead(POT_PIN);         // 0 .. 4095
  int duty = raw / 16;                    // 0 .. 255
  if (duty > 255) duty = 255;             // guard the top end of the range

  //MPU READING
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);           // acceleration in m/s^2

  if (a.acceleration.z > 8) analogWrite(LED_PIN, duty); 
  //NEED TO 
  //use alignment in logic positive Z value
  //factor out shake values only when delta reads < .20 
  //
  else analogWrite(LED_PIN, 0);

  //POT PRINTING
  Serial.printf("raw %4d -> duty %3d\n", raw, duty);

  //MPU PRINTING
  Serial.printf("%.2f,%.2f,%.2f\n",
                a.acceleration.x, a.acceleration.y, a.acceleration.z);

  delay(50);                          // ~50 Hz — the rate you'll train AND deploy at
}
