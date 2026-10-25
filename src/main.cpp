/*
 * Session 4 — run YOUR trained model on the board.
 * --------------------------------------------------------------------------
 * PREREQUISITE (do this first, or it will NOT compile — that's expected):
 *   1. In Edge Impulse: Deployment -> "Arduino library" -> EON + int8 -> Build.
 *   2. Unzip the downloaded .zip into the lib/ folder of this project.
 *   3. Change the #include below to match the header the zip created
 *      (look in lib/<your-project>/  -> it ends in _inferencing.h).
 *
 * This is the honest shape of on-device ML:
 *   read the sensor -> fill a window -> run the model -> act on the result.
 * No web server, no network — just the sensor and the model.
 */
#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

// 👇 CHANGE THIS to your Edge Impulse project's header:
#include <../lib/nik-etl-project-1_inferencing/src/nik-etl-project-1_inferencing.h>

#define LED_PIN 38 

Adafruit_MPU6050 mpu;

float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];   // the window — allocated once (50Hz x 2 sec windows x 3 axes = 300 float values)  
int   i = 0;                                        // fill position

// RGB color per class index (shake = red, wave = blue, idle = green)
const uint8_t colors[EI_CLASSIFIER_LABEL_COUNT][3] = {
  {0, 50, 0},   // idle
  {50, 0, 0},   // shake
  {0, 0, 50}    // wave
};


void act_on(ei_impulse_result_t &result) {
  // Pick the highest-confidence class.
  int   best = 0;
  float best_val = 0.0f;
  for (int k = 0; k < EI_CLASSIFIER_LABEL_COUNT; k++) { //iterate through classes
    if (result.classification[k].value > best_val) { // pick the highest-confidence class
      best_val = result.classification[k].value;  
      best = k;
    }
  }
  // Your threshold is a DECISION, not a default. Pick it for the error that
  // hurts your project more (a missed event vs a false trigger).
  if (best_val > 0.70f) {
    Serial.printf(">> %s (%.0f%%)\n", result.classification[best].label, best_val * 100);
    // TODO: your action here — light the RGB LED, move a servo, send a message.
    //change color RGB based on classification 
    neopixelWrite(LED_PIN, colors[best][0], colors[best][1], colors[best][2]); // passes the values from color index for each parameter of the RGB function
    if (best == 2) {   // 2 is wave's index 
      Serial.printf("\n\nHi\n\n");
    }
  }
}
void setup() {
  Serial.begin(115200);
  delay(300);
  Wire.begin(8, 9); // sda 8 scl 9 
  if (!mpu.begin()) { Serial.println("MPU not found"); while (true) delay(1000); }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
}

void loop() {
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  buffer[i++] = a.acceleration.x; //filling window buffer with acceleration data [x1,y1,z1,x2,y2,z2...]
  buffer[i++] = a.acceleration.y;
  buffer[i++] = a.acceleration.z; 

  if (i >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) { //when buffer fills up 
    signal_t signal;
    numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal); //absorb raw data/signal from buffer 

    ei_impulse_result_t result;

    uint32_t t0 = millis();
    run_classifier(&signal, &result, false); //run the classifier using the address of signal struct and outputting at the address of result struct
    uint32_t t1 = millis();

    Serial.printf("Inference time: %lu ms\n", (unsigned long)(t1 - t0)); 

    act_on(result);
    i = 0;
  }

  delay(20);   // 50 Hz — the rate you trained at! (drop this and the model sees garbage)
}

