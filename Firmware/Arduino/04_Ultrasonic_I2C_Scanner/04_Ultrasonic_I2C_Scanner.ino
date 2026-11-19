#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(8, 9);  // SDA = GPIO8, SCL = GPIO9

  Serial.println("Scanning I2C bus...");
}

void loop() {
  byte deviceCount = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C device at address 0x");

      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(address, HEX);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found.");
  }

  Serial.println("--------------------");
  delay(3000);
}