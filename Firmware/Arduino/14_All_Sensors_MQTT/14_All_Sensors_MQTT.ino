#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HX711.h>
#include <DHT.h>


// ======================================================
// WI-FI SETTINGS
// ======================================================

const char* WIFI_SSID = "Mahfuz";
const char* WIFI_PASSWORD = "M@hfuz084";


// ======================================================
// MQTT SETTINGS
// ======================================================

const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char* TELEMETRY_TOPIC =
  "savonia/mahfuz/iot_thesis/telemetry";

const char* RFID_TOPIC =
  "savonia/mahfuz/iot_thesis/rfid";


// ======================================================
// PIN CONFIGURATION
// ======================================================

// HX711
#define HX711_DOUT 4
#define HX711_SCK  5

// DHT22
#define DHT_PIN  7
#define DHT_TYPE DHT22

// Ultrasonic I2C
#define I2C_SDA 8
#define I2C_SCL 9
#define ULTRASONIC_ADDR 0x57

// RC522 RFID
#define RFID_SS   10
#define RFID_MOSI 11
#define RFID_SCK  12
#define RFID_MISO 13
#define RFID_RST  14


// ======================================================
// OBJECTS
// ======================================================

HX711 scale;

DHT dht(DHT_PIN, DHT_TYPE);

MFRC522 rfid(RFID_SS, RFID_RST);

WiFiClient wifiClient;

PubSubClient mqttClient(wifiClient);


// ======================================================
// HX711 CALIBRATION
// ======================================================

float calibration_factor = -208.45;


// ======================================================
// TIMING
// ======================================================

unsigned long lastTelemetryPublish = 0;

const unsigned long TELEMETRY_INTERVAL = 5000;


// ======================================================
// HX711 STARTUP WAIT
// ======================================================

bool waitForHX711(unsigned long timeoutMs = 10000) {

  unsigned long startTime = millis();

  int attempt = 1;

  while (millis() - startTime < timeoutMs) {

    if (scale.is_ready()) {

      Serial.println("HX711 detected!");

      return true;
    }

    Serial.print("HX711 attempt ");
    Serial.print(attempt);
    Serial.println(": not ready");

    attempt++;

    delay(500);
  }

  Serial.println(
    "ERROR: HX711 not detected within timeout."
  );

  return false;
}


// ======================================================
// WI-FI CONNECTION
// ======================================================

void connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {

    return;
  }

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println(
    "Wi-Fi connected!"
  );

  Serial.print(
    "ESP32 IP: "
  );

  Serial.println(
    WiFi.localIP()
  );
}


// ======================================================
// MQTT CONNECTION
// ======================================================

void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print(
      "Connecting to MQTT..."
    );

    String clientId =
      "ESP32S3-INVENTORY-";

    clientId += String(
      (uint32_t)ESP.getEfuseMac(),
      HEX
    );

    if (
      mqttClient.connect(
        clientId.c_str()
      )
    ) {

      Serial.println(
        "connected!"
      );

    } else {

      Serial.print(
        "failed, state="
      );

      Serial.println(
        mqttClient.state()
      );

      delay(2000);
    }
  }
}


// ======================================================
// ULTRASONIC SENSOR
// ======================================================

float readUltrasonicCM() {

  Wire.beginTransmission(
    ULTRASONIC_ADDR
  );

  Wire.write(0x01);

  if (
    Wire.endTransmission() != 0
  ) {

    return -1;
  }

  delay(120);

  Wire.requestFrom(
    ULTRASONIC_ADDR,
    3
  );

  if (
    Wire.available() < 3
  ) {

    return -1;
  }

  uint32_t raw = 0;

  raw |= (
    (uint32_t)Wire.read()
    << 16
  );

  raw |= (
    (uint32_t)Wire.read()
    << 8
  );

  raw |= Wire.read();


  // Raw / 1000 = millimetres
  float distance_mm =
    raw / 1000.0;


  // Convert mm to cm
  float distance_cm =
    distance_mm / 10.0;


  return distance_cm;
}


// ======================================================
// RFID UID FUNCTION
// ======================================================

String getRFIDUID() {

  String uid = "";

  for (
    byte i = 0;
    i < rfid.uid.size;
    i++
  ) {

    if (i > 0) {

      uid += ":";
    }

    if (
      rfid.uid.uidByte[i] < 0x10
    ) {

      uid += "0";
    }

    uid += String(
      rfid.uid.uidByte[i],
      HEX
    );
  }

  uid.toUpperCase();

  return uid;
}


// ======================================================
// RFID EVENT
// ======================================================

void checkRFID() {

  if (
    !rfid.PICC_IsNewCardPresent()
  ) {

    return;
  }

  if (
    !rfid.PICC_ReadCardSerial()
  ) {

    return;
  }


  String uid =
    getRFIDUID();


  Serial.println();

  Serial.print(
    "RFID detected: "
  );

  Serial.println(uid);


  // ------------------------------
  // Create JSON
  // ------------------------------

  String payload = "{";

  payload +=
    "\"device_id\":\"esp32_s3_01\",";

  payload +=
    "\"event\":\"rfid_scan\",";

  payload +=
    "\"uid\":\"";

  payload += uid;

  payload += "\",";

  payload +=
    "\"uptime_ms\":";

  payload += String(
    millis()
  );

  payload += "}";


  // ------------------------------
  // Publish RFID event
  // ------------------------------

  bool published =
    mqttClient.publish(
      RFID_TOPIC,
      payload.c_str()
    );


  if (published) {

    Serial.print(
      "RFID MQTT published: "
    );

    Serial.println(
      payload
    );

  } else {

    Serial.println(
      "RFID MQTT publish FAILED."
    );
  }


  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}


// ======================================================
// TELEMETRY FUNCTION
// ======================================================

void publishTelemetry() {

  Serial.println();

  Serial.println(
    "-------- SENSOR TELEMETRY --------"
  );


  // ==================================================
  // DHT22
  // ==================================================

  float temperature =
    dht.readTemperature();

  float humidity =
    dht.readHumidity();


  // ==================================================
  // ULTRASONIC
  // ==================================================

  float distance =
    readUltrasonicCM();


  // ==================================================
  // HX711
  // ==================================================

  float weight = 0.0;

  bool hxReady =
    scale.is_ready();


  if (hxReady) {

    // Average 20 HX711 measurements
    weight =
      scale.get_units(20);
  }


  // ==================================================
  // SERIAL OUTPUT
  // ==================================================

  if (
    isnan(temperature) ||
    isnan(humidity)
  ) {

    Serial.println(
      "DHT22       : READ ERROR"
    );

  } else {

    Serial.print(
      "Temperature : "
    );

    Serial.print(
      temperature,
      1
    );

    Serial.println(" C");


    Serial.print(
      "Humidity    : "
    );

    Serial.print(
      humidity,
      1
    );

    Serial.println(" %");
  }


  if (
    distance < 0
  ) {

    Serial.println(
      "Distance    : READ ERROR"
    );

  } else {

    Serial.print(
      "Distance    : "
    );

    Serial.print(
      distance,
      1
    );

    Serial.println(
      " cm"
    );
  }


  if (hxReady) {

    Serial.print(
      "Weight      : "
    );

    Serial.print(
      weight,
      1
    );

    Serial.println(
      " g"
    );

  } else {

    Serial.println(
      "Weight      : HX711 NOT READY"
    );
  }


  // ==================================================
  // JSON PAYLOAD
  // ==================================================

  String payload = "{";


  // Device ID
  payload +=
    "\"device_id\":\"esp32_s3_01\",";


  // Temperature
  payload +=
    "\"temperature_c\":";

  if (
    isnan(temperature)
  ) {

    payload += "null";

  } else {

    payload += String(
      temperature,
      1
    );
  }

  payload += ",";


  // Humidity
  payload +=
    "\"humidity_pct\":";

  if (
    isnan(humidity)
  ) {

    payload += "null";

  } else {

    payload += String(
      humidity,
      1
    );
  }

  payload += ",";


  // Distance
  payload +=
    "\"distance_cm\":";

  if (
    distance < 0
  ) {

    payload += "null";

  } else {

    payload += String(
      distance,
      1
    );
  }

  payload += ",";


  // Weight
  payload +=
    "\"weight_g\":";

  if (
    !hxReady
  ) {

    payload += "null";

  } else {

    payload += String(
      weight,
      1
    );
  }

  payload += ",";


  // Uptime
  payload +=
    "\"uptime_ms\":";

  payload += String(
    millis()
  );


  payload += "}";


  // ==================================================
  // MQTT PUBLISH
  // ==================================================

  bool published =
    mqttClient.publish(
      TELEMETRY_TOPIC,
      payload.c_str()
    );


  if (published) {

    Serial.print(
      "Telemetry published: "
    );

    Serial.println(
      payload
    );

  } else {

    Serial.println(
      "Telemetry MQTT publish FAILED."
    );
  }


  Serial.println(
    "----------------------------------"
  );
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(
    115200
  );

  delay(
    1000
  );


  Serial.println();

  Serial.println(
    "======================================"
  );

  Serial.println(
    "      ALL SENSORS + MQTT TEST"
  );

  Serial.println(
    "======================================"
  );


  // ==================================================
  // DHT22
  // ==================================================

  dht.begin();

  Serial.println(
    "DHT22 initialized."
  );


  // ==================================================
  // I2C / ULTRASONIC
  // ==================================================

  Wire.begin(
    I2C_SDA,
    I2C_SCL
  );

  Serial.println(
    "I2C initialized."
  );


  // ==================================================
  // RC522
  // ==================================================

  SPI.begin(
    RFID_SCK,
    RFID_MISO,
    RFID_MOSI,
    RFID_SS
  );


  rfid.PCD_Init();


  delay(
    100
  );


  // Turn RFID antenna on
  rfid.PCD_AntennaOn();


  // Maximum receiver gain
  rfid.PCD_SetAntennaGain(
    MFRC522::RxGain_max
  );


  byte version =
    rfid.PCD_ReadRegister(
      MFRC522::VersionReg
    );


  Serial.print(
    "RC522 Version: 0x"
  );

  Serial.println(
    version,
    HEX
  );


  if (
    version == 0x91 ||
    version == 0x92
  ) {

    Serial.println(
      "RC522 detected successfully."
    );

  } else {

    Serial.println(
      "WARNING: RC522 communication problem."
    );
  }


  // ==================================================
  // HX711 INITIALIZATION
  // ==================================================

  scale.begin(
    HX711_DOUT,
    HX711_SCK
  );


  Serial.println();

  Serial.println(
    "Waiting for HX711..."
  );


  bool hxDetected =
    waitForHX711(10000);


  if (hxDetected) {

    scale.set_scale(
      calibration_factor
    );


    Serial.println(
      "HX711 initialized."
    );


    Serial.println(
      "Tare will happen AFTER Wi-Fi/MQTT startup."
    );

  } else {

    Serial.println(
      "HX711 initialization failed."
    );
  }


  // ==================================================
  // WI-FI
  // ==================================================

  connectWiFi();


  // ==================================================
  // MQTT
  // ==================================================

  mqttClient.setServer(
    MQTT_BROKER,
    MQTT_PORT
  );


  connectMQTT();


  // ==================================================
  // STABILIZATION BEFORE TARE
  // ==================================================

  Serial.println();

  Serial.println(
    "Keep load cell COMPLETELY EMPTY."
  );


  Serial.println(
    "Waiting for electronics to stabilize..."
  );


  delay(
    3000
  );


  // ==================================================
  // HX711 TARE
  // ==================================================

  if (
    waitForHX711(10000)
  ) {

    Serial.println(
      "Taring HX711 with Wi-Fi active..."
    );


    scale.tare(
      20
    );


    Serial.println(
      "HX711 tare complete."
    );

  } else {

    Serial.println(
      "ERROR: HX711 not ready for tare."
    );
  }


  // ==================================================
  // SYSTEM READY
  // ==================================================

  Serial.println();

  Serial.println(
    "======================================"
  );

  Serial.println(
    "FULL SYSTEM READY"
  );

  Serial.println(
    "======================================"
  );


  Serial.println(
    "Scan RFID tag at any time."
  );

  Serial.println(
    "Telemetry publishes every 5 seconds."
  );
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // ==================================================
  // WI-FI RECOVERY
  // ==================================================

  if (
    WiFi.status() != WL_CONNECTED
  ) {

    connectWiFi();
  }


  // ==================================================
  // MQTT RECOVERY
  // ==================================================

  if (
    !mqttClient.connected()
  ) {

    connectMQTT();
  }


  mqttClient.loop();


  // ==================================================
  // RFID
  // ==================================================

  checkRFID();


  // ==================================================
  // PERIODIC SENSOR TELEMETRY
  // ==================================================

  if (
    millis() -
    lastTelemetryPublish
    >=
    TELEMETRY_INTERVAL
  ) {

    lastTelemetryPublish =
      millis();


    publishTelemetry();
  }
}