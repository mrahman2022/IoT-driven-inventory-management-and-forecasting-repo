#include <DHT.h>
#include <Wire.h>

// -------------------- DHT22 configuration --------------------

#define DHT_PIN 7
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

// ---------------- Ultrasonic configuration ------------------

#define ULTRASONIC_ADDRESS 0x57

const int SDA_PIN = 8;
const int SCL_PIN = 9;

// ---------------------- Timing -------------------------------

unsigned long previousDHTTime = 0;
unsigned long previousUltrasonicTime = 0;

const unsigned long DHT_INTERVAL = 2000;
const unsigned long ULTRASONIC_INTERVAL = 500;

// ---------------- Sensor-reading functions ------------------

void readDHT22() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("[DHT22] Failed to read sensor");
    return;
  }

  Serial.print("[DHT22] Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");
}

void readUltrasonic() {
  Wire.beginTransmission(ULTRASONIC_ADDRESS);
  Wire.write(0x01);

  byte transmissionResult = Wire.endTransmission();

  if (transmissionResult != 0) {
    Serial.println("[Ultrasonic] Measurement request failed");
    return;
  }

  delay(120);

  int receivedBytes = Wire.requestFrom(ULTRASONIC_ADDRESS, 3);

  if (receivedBytes == 3 && Wire.available() >= 3) {
    uint32_t rawData = 0;

    rawData |= ((uint32_t)Wire.read() << 16);
    rawData |= ((uint32_t)Wire.read() << 8);
    rawData |= Wire.read();

    float distanceMm = rawData / 1000.0;
    float distanceCm = distanceMm / 10.0;

    if (distanceMm >= 20.0 && distanceMm <= 4500.0) {
      Serial.print("[Ultrasonic] Distance: ");
      Serial.print(distanceCm, 1);
      Serial.println(" cm");
    } else {
      Serial.print("[Ultrasonic] Reading outside valid range: ");
      Serial.print(distanceMm, 1);
      Serial.println(" mm");
    }
  } else {
    Serial.println("[Ultrasonic] Failed to receive data");
  }
}

// ---------------------- Setup -------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Multi-sensor test started");
  Serial.println("--------------------------------");
}

// ----------------------- Loop -------------------------------

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousDHTTime >= DHT_INTERVAL) {
    previousDHTTime = currentTime;
    readDHT22();
  }

  if (currentTime - previousUltrasonicTime >= ULTRASONIC_INTERVAL) {
    previousUltrasonicTime = currentTime;
    readUltrasonic();
  }
}