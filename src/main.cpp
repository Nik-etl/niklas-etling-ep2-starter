/*
 * Embedded Programming 2 — Session 1: your first flash
 * --------------------------------------------------------------------------
 * The smallest thing that proves the WHOLE chain works:
 *   your code -> compiler -> USB -> real silicon -> and back to your screen.
 *
 * About the LED: this board has no ordinary on/off LED. It carries a single
 * addressable RGB LED (a WS2812), so you don't switch it on — you send it a
 * colour. neopixelWrite(pin, red, green, blue) does that for you.
 *
 * Check the silkscreen next to the LED for its pin! Ours says "RGB@IO38".
 * Other ESP32-S3 boards put it on GPIO48 — if yours does, change the line
 * below. (This is why LED_BUILTIN doesn't help here: it assumes GPIO48.)
 *
 * The serial "hello" is the guaranteed proof: if you see it in the monitor,
 * your toolchain, your cable and your board are all fine.
 *
 * SCOPE_PIN is for the PicoScope block later in the session: a plain 1 kHz
 * square wave, always running, that you can find on the scope in seconds.
 * It is deliberately NOT the LED pin — see the note above ledcSetup().
 */
#include <Arduino.h>

#define RGB_LED_PIN 38   // silkscreen: RGB@IO38
#define SCOPE_PIN    2   // J3 pin 5, four pins down from GND — probe this one

void setup() {
  Serial.begin(115200);
  delay(300);                                 // let serial come up before printing
  Serial.println();
  Serial.println("Hello from ESP32-S3 — the whole chain works!");

  /*
   * A test signal for the oscilloscope.
   *
   * Why a separate pin, when the LED is already blinking? Because the LED is
   * a WS2812: its pin sits idle-low and carries a ~30 us burst of pulses only
   * at the instant the colour changes. Twice a second, for 30 microseconds —
   * that is 0.006 % of the time. Finding that on a scope is a genuinely hard
   * first capture. GPIO2 gives you an easy one: a steady square wave that is
   * always there.
   *
   * LEDC is the ESP32's hardware PWM. Once set up it runs on its own, so the
   * delay(500) calls in loop() below don't disturb it at all.
   */
  ledcSetup(0, 1000, 10);        // channel 0, 1 kHz, 10-bit duty resolution
  ledcAttachPin(SCOPE_PIN, 0);
  ledcWrite(0, 512);             // 512 / 1023 = 50 % duty -> a square wave
  Serial.println("Scope test signal: 1 kHz square wave on GPIO2");
}

void loop() {
  neopixelWrite(RGB_LED_PIN, 0, 24, 0);       // dim green (r, g, b)
  Serial.println("blink: on");
  delay(500);

  neopixelWrite(RGB_LED_PIN, 0, 0, 0);        // off
  Serial.println("blink: off");
  delay(500);
}
