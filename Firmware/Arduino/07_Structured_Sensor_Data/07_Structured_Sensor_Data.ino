#include <DHT.h>
#include <Wire.h>

// -------------------- Pin configuration --------------------

#define DHT_PIN 7
#define DHT_TYPE DHT22

const int SDA_PIN = 8;
const int SCL_PIN = 9;

// ---------------- Sensor configuration ---------------------

#define ULTRASONIC_ADDRESS 0x57

DHT dht(DHT_PIN, DHT_TYPE);

// -------------------- Data structure -----------------------

struct SensorData {
  float temperature;
  float humidity;
  float distanceCm;

  bool dhtValid;
  bool ultrasonicValid;
};

SensorData sensorData = {
  0.0,
  0.0,
  0.0,
  false,
  false
};

// ---------------------- Timing -----------------------------

unsigned long previousDHTTime = 0;
unsigned long previousUltrasonicTime = 0;
unsigned long previousOutputTime = 0;

const unsigned long DHT_INTERVAL = 2000;
const unsigned long ULTRASONIC_INTERVAL = 500;
const unsigned long OUTPUT_INTERVAL = 2000;

// -------------------- DHT22 function -----------------------

void updateDHT22() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    sensorData.dhtValid = false;
    return;
  }

  sensorData.temperature = temperature;
  sensorData.humidity = humidity;
  sensorData.dhtValid = true;
}

// ---------------- Ultrasonic function ----------------------

void updateUltrasonic() {
  Wire.beginTransmission(ULTRASONIC_ADDRESS);
  Wire.write(0x01);

  byte transmissionResult = Wire.endTransmission();

  if (transmissionResult != 0) {
    sensorData.ultrasonicValid = false;
    return;
  }

  delay(120);

  int receivedBytes = Wire.requestFrom(ULTRASONIC_ADDRESS, 3);

  if (receivedBytes != 3 || Wire.available() < 3) {
    sensorData.ultrasonicValid = false;
    return;
  }

  uint32_t rawData = 0;

  rawData |= ((uint32_t)Wire.read() << 16);
  rawData |= ((uint32_t)Wire.read() << 8);
  rawData |= Wire.read();

  float distanceMm = rawData / 1000.0;
  float distanceCm = distanceMm / 10.0;

  if (distanceMm >= 20.0 && distanceMm <= 4500.0) {
    sensorData.distanceCm = distanceCm;
    sensorData.ultrasonicValid = true;
  } else {
    sensorData.ultrasonicValid = false;
  }
}

// -------------------- JSON output --------------------------

void printSensorDataAsJson() {
  Serial.print("{");

  Serial.print("\"temperature\":");
  if (sensorData.dhtValid) {
    Serial.print(sensorData.temperature, 1);
  } else {
    Serial.print("null");
  }

  Serial.print(",");

  Serial.print("\"humidity\":");
  if (sensorData.dhtValid) {
    Serial.print(sensorData.humidity, 1);
  } else {
    Serial.print("null");
  }

  Serial.print(",");

  Serial.print("\"distance_cm\":");
  if (sensorData.ultrasonicValid) {
    Serial.print(sensorData.distanceCm, 1);
  } else {
    Serial.print("null");
  }

  Serial.println("}");
}

// ------------------------ Setup ----------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Structured sensor data test started");
}

// ------------------------- Loop ----------------------------

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousDHTTime >= DHT_INTERVAL) {
    previousDHTTime = currentTime;
    updateDHT22();
  }

  if (currentTime - previousUltrasonicTime >= ULTRASONIC_INTERVAL) {
    previousUltrasonicTime = currentTime;
    updateUltrasonic();
  }

  if (currentTime - previousOutputTime >= OUTPUT_INTERVAL) {
    previousOutputTime = currentTime;
    printSensorDataAsJson();
  }
}