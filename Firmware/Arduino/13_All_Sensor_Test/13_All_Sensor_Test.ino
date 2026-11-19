#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HX711.h>
#include <DHT.h>

// ======================================================
// PIN CONFIGURATION
// ======================================================

// HX711
#define HX711_DOUT 4
#define HX711_SCK  5

// DHT22
#define DHT_PIN   7
#define DHT_TYPE  DHT22

// Ultrasonic I2C
#define I2C_SDA 8
#define I2C_SCL 9
#define ULTRASONIC_ADDR 0x57

// RC522
#define RFID_SS    10
#define RFID_MOSI  11
#define RFID_SCK   12
#define RFID_MISO  13
#define RFID_RST   14


// ======================================================
// OBJECTS
// ======================================================

HX711 scale;
DHT dht(DHT_PIN, DHT_TYPE);
MFRC522 rfid(RFID_SS, RFID_RST);


// HX711 calibration factor from our successful calibration
float calibration_factor = -208.45;


// ======================================================
// TIMING
// ======================================================

unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 2000;


// ======================================================
// ULTRASONIC FUNCTION
// ======================================================

float readUltrasonicCM() {

  // Start measurement
  Wire.beginTransmission(ULTRASONIC_ADDR);
  Wire.write(0x01);

  if (Wire.endTransmission() != 0) {
    return -1;
  }

  // Sensor measurement cycle
  delay(120);

  Wire.requestFrom(ULTRASONIC_ADDR, 3);

  if (Wire.available() < 3) {
    return -1;
  }

  uint32_t raw = 0;

  raw |= ((uint32_t)Wire.read() << 16);
  raw |= ((uint32_t)Wire.read() << 8);
  raw |= Wire.read();

  // Official sensor value:
  // raw / 1000 = millimetres
  float distance_mm = raw / 1000.0;

  return distance_mm / 10.0;   // convert mm → cm
}


// ======================================================
// RFID FUNCTION
// ======================================================

void checkRFID() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("\n>>> RFID UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1) {
      Serial.print(":");
    }
  }

  Serial.println(" <<<");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("======================================");
  Serial.println("      ALL SENSORS INTEGRATION TEST");
  Serial.println("======================================");


  // --------------------------------------------------
  // DHT22
  // --------------------------------------------------

  dht.begin();

  Serial.println("DHT22 initialized.");


  // --------------------------------------------------
  // I2C / Ultrasonic
  // --------------------------------------------------

  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("I2C initialized.");
  Serial.println("Ultrasonic address: 0x57");


  // --------------------------------------------------
  // RC522 RFID
  // --------------------------------------------------

  SPI.begin(
    RFID_SCK,
    RFID_MISO,
    RFID_MOSI,
    RFID_SS
  );

  rfid.PCD_Init();
  delay(100);

  byte version =
    rfid.PCD_ReadRegister(MFRC522::VersionReg);

  Serial.print("RC522 Version: 0x");
  Serial.println(version, HEX);

  if (version == 0x91 || version == 0x92) {
    Serial.println("RC522 detected successfully.");
  } else {
    Serial.println("WARNING: RC522 communication problem.");
  }


  // --------------------------------------------------
  // HX711
  // --------------------------------------------------

  scale.begin(HX711_DOUT, HX711_SCK);

  if (scale.is_ready()) {

    Serial.println("HX711 detected.");

    scale.set_scale(calibration_factor);

    Serial.println("Keep load cell EMPTY.");
    Serial.println("Taring...");
    delay(3000);

    scale.tare();

    Serial.println("HX711 tare complete.");

  } else {

    Serial.println("WARNING: HX711 not detected.");
  }


  Serial.println();
  Serial.println("======================================");
  Serial.println("SYSTEM READY");
  Serial.println("Scan RFID tag at any time.");
  Serial.println("======================================");
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // Check RFID continuously
  checkRFID();


  // Read remaining sensors every 2 seconds
  if (millis() - lastSensorRead >= SENSOR_INTERVAL) {

    lastSensorRead = millis();

    Serial.println();
    Serial.println("-------- SENSOR READINGS --------");


    // --------------------------------------------------
    // DHT22
    // --------------------------------------------------

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {

      Serial.println("DHT22: READ ERROR");

    } else {

      Serial.print("Temperature : ");
      Serial.print(temperature, 1);
      Serial.println(" C");

      Serial.print("Humidity    : ");
      Serial.print(humidity, 1);
      Serial.println(" %");
    }


    // --------------------------------------------------
    // Ultrasonic
    // --------------------------------------------------

    float distance = readUltrasonicCM();

    if (distance < 0) {

      Serial.println("Distance    : READ ERROR");

    } else {

      Serial.print("Distance    : ");
      Serial.print(distance, 1);
      Serial.println(" cm");
    }


    // --------------------------------------------------
    // HX711
    // --------------------------------------------------

    if (scale.is_ready()) {

      float weight = scale.get_units(3);

      Serial.print("Weight      : ");
      Serial.print(weight, 1);
      Serial.println(" g");

    } else {

      Serial.println("Weight      : HX711 NOT READY");
    }


    Serial.println("---------------------------------");
  }
}