#include "HX711.h"

#define HX711_DOUT 4
#define HX711_SCK  5

HX711 scale;

// Initial calibration factor calculated using 1000 g reference weight
float calibration_factor = -208.45;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== HX711 Calibrated Weight Test ===");

  scale.begin(HX711_DOUT, HX711_SCK);

  if (!scale.is_ready()) {
    Serial.println("ERROR: HX711 not detected!");
    while (1);
  }

  Serial.println("HX711 detected.");

  scale.set_scale(calibration_factor);

  Serial.println("Remove all weight from the load cell.");
  Serial.println("Taring...");
  delay(3000);

  scale.tare();

  Serial.println("Tare complete.");
  Serial.println("Place weight on the load cell.");
  Serial.println();
}

void loop() {

  if (scale.is_ready()) {

    float weight = scale.get_units(10);

    Serial.print("Weight: ");
    Serial.print(weight, 1);
    Serial.println(" g");

  } else {

    Serial.println("HX711 not ready.");
  }

  delay(1000);
}