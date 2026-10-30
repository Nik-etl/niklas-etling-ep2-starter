/*
 * Session 6 — run a person / no-person model on the board, and time it.
 * --------------------------------------------------------------------------
 * PREREQUISITE (do this first, or it will NOT compile — that's expected):
 *   1. In Edge Impulse: Deployment -> "Arduino library" -> TensorFlow Lite
 *      + Quantized (int8) -> Build.
 *   2. Unzip the downloaded .zip into the lib/ folder of this project.
 *   3. Change the #include below to match the header the zip created
 *      (look in lib/<your-project>_inferencing/ -> it ends in _inferencing.h).
 *   4. In platformio.ini, add this line under build_flags:
 *          -D EI_CLASSIFIER_TFLITE_ENABLE_ESP_NN=0
 *      It switches off the ESP32-S3's fast vision kernels (ESP-NN). They are
 *      much faster, but they need more memory than Edge Impulse plans for, so
 *      with them on only some image sizes fit. Off, all three sizes run and
 *      you can compare them fairly. Turning them back on is extra practice.
 *
 * No camera: two photos are baked into include/test_images.h as arrays of
 * pixels (one with a person, one of the same room empty). The model cannot
 * tell the difference — it does the same arithmetic and takes just as long.
 *
 * Retrained at another image size (48 or 160 px)? Nothing to change here:
 * the sketch picks the matching test image from the model's own settings.
 */
#include <Arduino.h>
#include "test_images.h"

// 👇 CHANGE THIS to your Edge Impulse project's header:
#include <your_project_inferencing.h>

// Pick the test images that match the model's input size.
#if EI_CLASSIFIER_INPUT_WIDTH == 48
  #define PERSON_IMAGE person_48
  #define EMPTY_IMAGE  empty_48
#elif EI_CLASSIFIER_INPUT_WIDTH == 96
  #define PERSON_IMAGE person_96
  #define EMPTY_IMAGE  empty_96
#elif EI_CLASSIFIER_INPUT_WIDTH == 160
  #define PERSON_IMAGE person_160
  #define EMPTY_IMAGE  empty_160
#else
  #error "test_images.h has 48, 96 and 160 px only. Regenerate it with tools/image_to_header.py"
#endif

static const uint8_t *current_image = nullptr;

// The model asks for pixels in chunks. Each pixel goes in as ONE number with
// red, green and blue packed together: 0xRRGGBB. That is the format Edge Impulse
// shows as "raw features" on the Image page.
static int get_pixels(size_t offset, size_t length, float *out) {
  for (size_t i = 0; i < length; i++) {
    size_t p = (offset + i) * 3;                       // 3 bytes per pixel: r, g, b
    out[i] = (float)((current_image[p] << 16) | (current_image[p + 1] << 8) | current_image[p + 2]);
  }
  return 0;
}

void classify(const char *name, const uint8_t *image) {
  current_image = image;

  signal_t signal;
  signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal.get_data = &get_pixels;

  ei_impulse_result_t result;
  unsigned long t0 = millis();
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  unsigned long ms = millis() - t0;                    // how long did ONE image take?

  if (err != EI_IMPULSE_OK) {                          // e.g. not enough memory for this size
    Serial.printf("%-7s run_classifier failed (error %d) after %lu ms\n", name, err, ms);
    return;
  }

  Serial.printf("%-7s", name);
  for (int k = 0; k < EI_CLASSIFIER_LABEL_COUNT; k++) {
    Serial.printf("  %s %.2f", result.classification[k].label, result.classification[k].value);
  }
  Serial.printf("  | %lu ms (%.1f FPS)\n", ms, 1000.0 / ms);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.printf("\nModel input: %d x %d px\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
  Serial.printf("PSRAM: %u bytes  (0 = switched off, see the session-6 slide)\n", (unsigned)ESP.getPsramSize());
  Serial.printf("Free internal RAM: %u bytes\n\n", (unsigned)ESP.getFreeHeap());
}

void loop() {
  classify("person", PERSON_IMAGE);
  classify("empty", EMPTY_IMAGE);
  Serial.println();
  delay(2000);
}
