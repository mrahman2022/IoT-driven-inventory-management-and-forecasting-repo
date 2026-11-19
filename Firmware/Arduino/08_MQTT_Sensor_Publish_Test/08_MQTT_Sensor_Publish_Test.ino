#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>

// ---------------- Wi-Fi credentials ----------------

const char* WIFI_SSID = "Mahfuz";
const char* WIFI_PASSWORD = "M@hfuz084";

// ---------------- MQTT configuration ---------------

const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// Use a unique topic to reduce conflicts with other users.
const char* MQTT_TOPIC =
    "savonia/mahfuz/iot_thesis/sensor_data";

// ---------------- Hardware configuration -----------

#define DHT_PIN 7
#define DHT_TYPE DHT22
#define ULTRASONIC_ADDRESS 0x57

const int SDA_PIN = 8;
const int SCL_PIN = 9;

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ---------------- Sensor data structure ------------

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

// ---------------- Timing configuration -------------

unsigned long previousDHTTime = 0;
unsigned long previousUltrasonicTime = 0;
unsigned long previousPublishTime = 0;

const unsigned long DHT_INTERVAL = 2000;
const unsigned long ULTRASONIC_INTERVAL = 500;
const unsigned long PUBLISH_INTERVAL = 5000;

// ---------------- Wi-Fi function -------------------

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

// ---------------- MQTT function --------------------

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker...");

    String clientId = "ESP32S3-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, state=");
      Serial.println(mqttClient.state());

      delay(2000);
    }
  }
}

// ---------------- DHT22 function -------------------

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

// ---------------- Ultrasonic function --------------

void updateUltrasonic() {
  Wire.beginTransmission(ULTRASONIC_ADDRESS);
  Wire.write(0x01);

  byte transmissionResult = Wire.endTransmission();

  if (transmissionResult != 0) {
    sensorData.ultrasonicValid = false;
    return;
  }

  delay(120);

  int receivedBytes =
      Wire.requestFrom(ULTRASONIC_ADDRESS, 3);

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

// ---------------- JSON creation --------------------

String createSensorJson() {
  String json = "{";

  json += "\"device_id\":\"esp32_s3_01\",";

  json += "\"temperature\":";
  if (sensorData.dhtValid) {
    json += String(sensorData.temperature, 1);
  } else {
    json += "null";
  }

  json += ",\"humidity\":";
  if (sensorData.dhtValid) {
    json += String(sensorData.humidity, 1);
  } else {
    json += "null";
  }

  json += ",\"distance_cm\":";
  if (sensorData.ultrasonicValid) {
    json += String(sensorData.distanceCm, 1);
  } else {
    json += "null";
  }

  json += ",\"uptime_ms\":";
  json += String(millis());

  json += "}";

  return json;
}

// ---------------- Publish function -----------------

void publishSensorData() {
  String payload = createSensorJson();

  bool published = mqttClient.publish(
      MQTT_TOPIC,
      payload.c_str()
  );

  if (published) {
    Serial.print("Published: ");
    Serial.println(payload);
  } else {
    Serial.println("MQTT publish failed");
  }
}

// ---------------- Setup ----------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Wire.begin(SDA_PIN, SCL_PIN);

  connectToWiFi();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  Serial.println("MQTT sensor publishing test started");
}

// ---------------- Loop -----------------------------

void loop() {
  unsigned long currentTime = millis();

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  mqttClient.loop();

  if (currentTime - previousDHTTime >= DHT_INTERVAL) {
    previousDHTTime = currentTime;
    updateDHT22();
  }

  if (
      currentTime - previousUltrasonicTime
      >= ULTRASONIC_INTERVAL
  ) {
    previousUltrasonicTime = currentTime;
    updateUltrasonic();
  }

  if (
      currentTime - previousPublishTime
      >= PUBLISH_INTERVAL
  ) {
    previousPublishTime = currentTime;
    publishSensorData();
  }
}