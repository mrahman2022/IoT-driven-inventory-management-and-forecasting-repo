#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "Mahfuz";
const char* WIFI_PASSWORD = "M@hfuz084";

// ---------- MQTT ----------
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char* MQTT_TOPIC =
  "savonia/mahfuz/iot_thesis/weight";

// ---------- HX711 ----------
#define HX711_DOUT 4
#define HX711_SCK  5

HX711 scale;

// Calibration factor obtained from 1000 g reference
float calibration_factor = -208.45;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


void connectWiFi() {

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}


void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to MQTT...");

    String clientId = "ESP32S3-WEIGHT-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println("connected!");

    } else {

      Serial.print("failed, state=");
      Serial.println(mqttClient.state());

      delay(2000);
    }
  }
}


void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== HX711 + MQTT Weight Test ===");

  // Initialize HX711
  scale.begin(HX711_DOUT, HX711_SCK);

  if (!scale.is_ready()) {
    Serial.println("ERROR: HX711 not detected!");
    while (1);
  }

  Serial.println("HX711 detected.");

  scale.set_scale(calibration_factor);

  Serial.println("Remove all weight.");
  Serial.println("Taring...");
  delay(3000);

  scale.tare();

  Serial.println("Tare complete.");

  // Connect to Wi-Fi
  connectWiFi();

  // Configure MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  connectMQTT();

  Serial.println();
  Serial.println("System ready.");
}


void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  if (!scale.is_ready()) {
    Serial.println("HX711 not ready.");
    delay(1000);
    return;
  }

  // Average 10 HX711 measurements
  float weight = scale.get_units(10);

  Serial.print("Weight: ");
  Serial.print(weight, 1);
  Serial.println(" g");

  // Create JSON message
  String payload = "{";

  payload += "\"device_id\":\"esp32_s3_01\",";
  payload += "\"sensor\":\"load_cell\",";
  payload += "\"weight_g\":";
  payload += String(weight, 1);
  payload += ",";
  payload += "\"uptime_ms\":";
  payload += String(millis());

  payload += "}";

  bool published =
    mqttClient.publish(MQTT_TOPIC, payload.c_str());

  if (published) {

    Serial.print("Published: ");
    Serial.println(payload);

  } else {

    Serial.println("MQTT publish failed.");
  }

  Serial.println();

  delay(5000);
}