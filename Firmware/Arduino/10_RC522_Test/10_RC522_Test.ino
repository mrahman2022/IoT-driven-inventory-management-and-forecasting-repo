#include <SPI.h>
#include <MFRC522.h>

// RC522 pins
#define SS_PIN    10
#define RST_PIN   14

#define SCK_PIN   12
#define MISO_PIN  13
#define MOSI_PIN  11

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== RC522 RFID Test ===");

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

  rfid.PCD_Init();
  delay(100);

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);

  Serial.print("RC522 Version Register: 0x");
  Serial.println(version, HEX);

  if (version == 0x91 || version == 0x92) {
    Serial.println("SUCCESS: RC522 detected.");
  }
  else if (version == 0x00 || version == 0xFF) {
    Serial.println("ERROR: RC522 not communicating.");
  }
  else {
    Serial.println("RC522 responded with unexpected version.");
  }

  Serial.println("Place RFID card/tag near reader...");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("RFID UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1) {
      Serial.print(":");
    }
  }

  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}