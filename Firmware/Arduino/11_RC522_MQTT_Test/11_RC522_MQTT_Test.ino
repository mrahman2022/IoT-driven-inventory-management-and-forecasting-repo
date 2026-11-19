#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "Mahfuz";
const char* WIFI_PASSWORD = "M@hfuz084";

// ---------- MQTT ----------
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char* MQTT_TOPIC =
  "savonia/mahfuz/iot_thesis/rfid";

// ---------- RC522 ----------
#define SS_PIN    10
#define RST_PIN   14
#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11

MFRC522 rfid(SS_PIN, RST_PIN);

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

    String clientId = "ESP32S3-RFID-";
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


String getUID() {

  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (i > 0) {
      uid += ":";
    }

    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();

  return uid;
}


void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== RC522 + MQTT Integration Test ===");

  // Start RFID
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  rfid.PCD_Init();
  delay(100);

  byte version =
    rfid.PCD_ReadRegister(MFRC522::VersionReg);

  Serial.print("RC522 Version: 0x");
  Serial.println(version, HEX);

  // Start Wi-Fi
  connectWiFi();

  // Configure MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  connectMQTT();

  Serial.println();
  Serial.println("System ready.");
  Serial.println("Scan an RFID tag...");
}


void loop() {

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  // Is a new RFID tag present?
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Can its UID be read?
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid = getUID();

  Serial.print("RFID detected: ");
  Serial.println(uid);

  // Create JSON message
  String payload = "{";
  payload += "\"device_id\":\"esp32_s3_01\",";
  payload += "\"event\":\"rfid_scan\",";
  payload += "\"uid\":\"";
  payload += uid;
  payload += "\",";
  payload += "\"uptime_ms\":";
  payload += String(millis());
  payload += "}";

  // Publish to MQTT
  bool published =
    mqttClient.publish(MQTT_TOPIC, payload.c_str());

  if (published) {

    Serial.println("MQTT publish successful:");

  } else {

    Serial.println("MQTT publish FAILED:");
  }

  Serial.println(payload);
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
}