/*
 * Session 2 HOMEWORK — analog in, PWM out: make the knob dim the LED.
 * --------------------------------------------------------------------------
 * This is the graded exercise for session 2:  FINAL: knob dims LED
 *
 * It is the smallest complete sense -> act loop in the whole course, and it is
 * the same shape as every project you will build: read something from the
 * world, decide a number, drive an output with it.
 *
 * WIRING (power off while you wire):
 *   pot outer leg 1 -> 3V3
 *   pot outer leg 2 -> GND
 *   pot middle leg  -> GPIO4
 *
 *   GPIO40 -> resistor (220-330 ohm) -> LED long leg (anode)
 *   LED short leg (cathode) -> GND
 *
 *   THE RESISTOR IS NOT OPTIONAL. An LED straight across a pin draws more
 *   current than the pin should give and can damage it. Long leg = positive.
 *
 * Same LED pin as session 1's plain-LED blink, so if you wired one then, it is
 * already in the right place — you are only adding the potentiometer.
 *
 * NOT the on-board RGB LED, and not LED_BUILTIN. Use the plain LED from your
 * box on a plain GPIO. (LED_BUILTIN assumes GPIO48 and is wrong for our board;
 * the on-board RGB one needs neopixelWrite() and colours, not brightness.)
 *
 * THE ONE BIT OF ARITHMETIC
 *   analogRead()  gives 0..4095   (12-bit input)
 *   analogWrite() wants 0..255    (8-bit output)
 *   4095 / 16 = 255, so dividing by 16 maps one range onto the other.
 *
 * Does this fight the 1 kHz scope signal on GPIO2 from session 1? No.
 * analogWrite() takes LEDC channels counting down from the top, and the scope
 * signal holds channel 0. They coexist.
 *
 * MAKE IT YOURS — that is the actual assignment:
 *   - invert it, so turning the knob up makes the LED dimmer
 *   - add a dead zone, so the bottom tenth of the travel is fully off
 *   - drive the buzzer instead of the LED (same idea, noisier)
 *   - print the raw value and the duty side by side and watch them track
 */
#include <Arduino.h>

#define POT_PIN 4
#define LED_PIN 40

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(LED_PIN, OUTPUT);
  Serial.println();
  Serial.println("Knob dims LED — turn the knob.");
}

void loop() {
  int raw  = analogRead(POT_PIN);         // 0 .. 4095
  int duty = raw / 16;                    // 0 .. 255
  if (duty > 255) duty = 255;             // guard the top end of the range

  analogWrite(LED_PIN, duty);

  Serial.printf("raw %4d -> duty %3d\n", raw, duty);
  delay(50);
}