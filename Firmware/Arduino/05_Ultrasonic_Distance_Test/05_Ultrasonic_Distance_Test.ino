#include <Wire.h>

#define ULTRASONIC_ADDRESS 0x57

const int SDA_PIN = 8;
const int SCL_PIN = 9;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Ultrasonic distance test started");
}

void loop() {
  Wire.beginTransmission(ULTRASONIC_ADDRESS);
  Wire.write(0x01);

  byte transmissionResult = Wire.endTransmission();

  if (transmissionResult != 0) {
    Serial.println("Failed to request measurement");
    delay(500);
    return;
  }

  delay(120);

  int receivedBytes = Wire.requestFrom(ULTRASONIC_ADDRESS, 3);

  if (receivedBytes == 3 && Wire.available() >= 3) {
    uint32_t rawData = Wire.read();

    rawData <<= 8;
    rawData |= Wire.read();

    rawData <<= 8;
    rawData |= Wire.read();

    float distanceMm = rawData / 1000.0;
    float distanceCm = distanceMm / 10.0;

    if (distanceMm >= 20.0 && distanceMm <= 4500.0) {
      Serial.print("Distance: ");
      Serial.print(distanceCm, 1);
      Serial.println(" cm");
    } else {
      Serial.print("Reading outside valid range: ");
      Serial.print(distanceMm, 1);
      Serial.println(" mm");
    }
  } else {
    Serial.println("Failed to read ultrasonic sensor");
  }

  delay(500);
}