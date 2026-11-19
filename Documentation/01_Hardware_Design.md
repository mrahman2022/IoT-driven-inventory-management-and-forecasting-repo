# Hardware Design

---

## Thesis Title

**IoT-Driven Inventory Management and Demand Forecasting Optimization Using Machine Learning**

---

# 1. Objective

The objective of this document is to define the hardware architecture of the proposed IoT-based inventory management system. It describes the main hardware components, sensors, communication interfaces, power requirements, and design principles used throughout the project. This document serves as the primary hardware reference and will be updated as new components are integrated and validated.

---

# 2. Hardware Overview

The hardware platform consists of an ESP32-S3 microcontroller for sensor data acquisition and a Raspberry Pi 5 for local data processing, storage, dashboard hosting, and machine learning inference.

| Device | Purpose | Status |
|---------|---------|--------|
| ESP32-S3 DevKitC-1 | Main microcontroller | ✅ Tested |
| Raspberry Pi 5 | Local server & Machine Learning | ⏳ Pending |
| Breadboard | Circuit prototyping | ✅ |
| Jumper wires | Hardware connections | ✅ |
| Mean Well 5V Power Supply | Stable power supply | ⏳ Planned |

---

# 3. Sensors and Modules

| Component | Power | Communication | ESP32 Connection | Purpose | Status |
|----------|----------|----------|----------|----------|--------|
| DHT22 | 3.3 V | Single-Wire Digital | DATA → GPIO7 | Temperature & Humidity Monitoring | ✅ Tested |
| PN532 NFC/RFID | 3.3 V | I²C | SDA=GPIO8, SCL=GPIO9 *(planned)* | Item Identification | ⏳ Pending |
| HX711 + Load Cell | 3.3 V | Digital | DT=?, SCK=? | Weight Measurement | ⏳ Pending |
| MaxBotix MB1010 | 5 V | Analog / PWM / UART | TBD | Distance Measurement | ⏳ Pending |
| MicroSD Breakout | 3.3 V | SPI | CS=?, MOSI=?, MISO=?, SCK=? | Local Data Storage | ⏳ Pending |

---

# 4. Hardware Architecture

```text
                 Environment
                       │
        ┌──────────────┼──────────────┐
        │              │              │
     DHT22         Load Cell      RFID Tag
        │              │              │
        └──────────────┼──────────────┘
                       │
                  ESP32-S3
                       │
                 Wi-Fi / MQTT
                       │
                Raspberry Pi 5
                       │
        Database • Machine Learning • Dashboard
```

---

# 5. Development Environment

| Software | Purpose |
|----------|----------|
| Arduino IDE | ESP32 Firmware Development |
| Visual Studio Code | Programming & Documentation |
| Git | Version Control |
| Python | Data Processing & Machine Learning |
| Raspberry Pi OS | Local Server Environment |

---

# 6. Hardware Design Principles

The following principles guide the hardware implementation throughout this project:

- ESP32 GPIO pins operate at **3.3 V logic**.
- Never connect **5 V directly to ESP32 GPIO pins**.
- Each hardware module must be tested independently before system integration.
- Wiring should remain modular to simplify debugging and future expansion.
- All wiring configurations, GPIO assignments, and test results shall be documented.
- Hardware modifications must be reflected in this document to maintain an accurate system specification.

---

# 7. Current Hardware Status

| Hardware Component | Tested | Integrated |
|-------------------|---------|------------|
| ESP32-S3 | ✅ | ✅ |
| DHT22 | ✅ | ✅ |
| PN532 | ⏳ | ⏳ |
| HX711 | ⏳ | ⏳ |
| Load Cell | ⏳ | ⏳ |
| MaxBotix MB1010 | ⏳ | ⏳ |
| Raspberry Pi 5 | ⏳ | ⏳ |
| Complete Hardware System | ⏳ | ⏳ |

---

# 8. Version History

| Version | Date | Description |
|----------|------------|----------------------------|
| 1.0 | 18 July 2026 | Initial hardware design document created. DHT22 successfully tested with ESP32-S3. |

---

**Document Status:** Active Development

**Last Updated:** 18 July 2026


## HX711 Load Cell Interface

The HX711 load cell amplifier was connected to the ESP32-S3 to verify basic communication before full load-cell calibration.

### ESP32-S3 Connections

| HX711 Pin | ESP32-S3 |
|-----------|-----------|
| VDD | 3.3V |
| VCC | 5V |
| GND | GND |
| DAT | GPIO4 |
| CLK | GPIO5 |

The HX711 communication test was successful. Raw ADC values were continuously received by the ESP32-S3, with readings approximately in the range of -1650 to -1670 during the test.

An initial wiring problem caused abnormal heating of the ESP32-S3. The circuit was disconnected, the ESP32-S3 was tested independently, and the HX711 wiring was checked and rebuilt. After correction, the ESP32-S3 and HX711 operated normally.

The HX711 interface is therefore considered functional. Full load-cell measurement and calibration are postponed to a later stage.

**Status:** Communication verified; calibration pending.


## RFID-RC522 Interface

The RFID-RC522 module was connected to the ESP32-S3 using SPI communication.

### ESP32-S3 Connections

| RC522 Pin | ESP32-S3 |
|-----------|-----------|
| SDA / SS | GPIO10 |
| SCK | GPIO12 |
| MOSI | GPIO11 |
| MISO | GPIO13 |
| IRQ | Not connected |
| GND | GND |
| RST | GPIO14 |
| 3.3V | 3.3V |

The RC522 was initialized successfully using the MFRC522 Arduino library.

The reader returned the following version register value:

`0x92`

This confirmed successful communication between the ESP32-S3 and the RC522.

An RFID test tag was then scanned successfully. The detected UID was:

`23:31:7F:33`

This verifies that the ESP32-S3 can detect RFID tags and retrieve their unique identifiers.

The RFID interface is ready for integration with the MQTT communication system.

**Status:** Communication and RFID tag detection successfully verified.



## Final Integrated ESP32-S3 Sensor Configuration

Following individual component testing, the DHT22, M5Stack Ultrasonic-I2C sensor, HX711 load-cell system, and RC522 RFID reader were integrated on a single ESP32-S3.

### Final Pin Assignment

| GPIO | Function |
|------|----------|
| GPIO4 | HX711 DAT |
| GPIO5 | HX711 CLK |
| GPIO7 | DHT22 DATA |
| GPIO8 | Ultrasonic SDA |
| GPIO9 | Ultrasonic SCL |
| GPIO10 | RC522 SDA / SS |
| GPIO11 | RC522 MOSI |
| GPIO12 | RC522 SCK |
| GPIO13 | RC522 MISO |
| GPIO14 | RC522 RST |

### Power Distribution

- DHT22: 3.3V
- RC522: 3.3V
- Ultrasonic-I2C: 5V
- HX711 VDD: 3.3V
- HX711 VCC: 5V
- Common ground shared by all devices

The ultrasonic module was successfully detected at I2C address `0x57`.

The RC522 returned version register `0x92` and successfully detected RFID UID:

`23:31:7F:33`

The HX711 retained the calibrated factor:

`-208.45`

During the final simultaneous test, representative measurements were:

- Temperature: approximately `22.7 °C`
- Humidity: approximately `56.0 %`
- Distance: approximately `55.5 cm`
- Weight with 1000 g reference: approximately `999.0–999.3 g`
- RFID UID: `23:31:7F:33`

All four subsystems operated successfully within the same ESP32-S3 program.

**Status: Multi-sensor hardware integration successfully completed.**



### Full Sensor and MQTT Integration Update

The final ESP32-S3 hardware configuration was successfully combined with Wi-Fi and MQTT communication.

The system simultaneously supported:

- DHT22 temperature and humidity sensing
- M5Stack Ultrasonic-I2C distance sensing
- HX711 calibrated weight measurement
- RC522 RFID identification
- Wi-Fi communication
- MQTT telemetry publishing
- MQTT RFID event publishing

The HX711 required a startup readiness check before initialization. The final firmware waits for the HX711 to become ready, establishes Wi-Fi and MQTT communication, allows the system to stabilize, and then performs a 20-sample tare.

Weight measurements are also averaged across 20 HX711 samples.

During the final full-system test, representative results were:

- Temperature: `22.1 °C`
- Humidity: `55.7 %`
- Distance: `56.3 cm`
- Weight with 1000 g reference: `999.5 g`
- RFID UID: `23:31:7F:33`

The system successfully published both continuous sensor telemetry and RFID events through MQTT.

**Status: Full ESP32-S3 sensor and MQTT integration successfully completed.**