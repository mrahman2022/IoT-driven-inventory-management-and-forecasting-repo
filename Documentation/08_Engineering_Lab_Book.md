# Engineering Lab Book

## Thesis Title

**IoT-Driven Inventory Management and Demand Forecasting Optimization Using Machine Learning**

---

## Experiment 01 — ESP32-S3 and DHT22 Sensor Integration

### Date

18 July 2026

### Status

✅ Completed

---

### 1. Objective

The objective of this experiment was to establish communication between the ESP32-S3 DevKitC-1 and the DHT22 temperature and humidity sensor.

The experiment also aimed to verify:

- correct sensor wiring;
- successful firmware compilation and upload;
- communication between the DHT22 and ESP32-S3;
- serial communication between the ESP32-S3 and the development computer;
- stable temperature and humidity measurements.

---

### 2. Hardware Used

| Component | Purpose |
|---|---|
| ESP32-S3 DevKitC-1 | Main microcontroller |
| DHT22 | Temperature and humidity measurement |
| Breadboard | Circuit prototyping |
| Jumper wires | Electrical connections |
| USB-C cable | Programming and serial communication |
| Development computer | Arduino programming and output monitoring |

---

### 3. Software Used

| Software or Library | Purpose |
|---|---|
| Arduino IDE | Firmware development and upload |
| ESP32 Arduino Core | ESP32-S3 board support |
| DHT Sensor Library by Adafruit | Reading DHT22 measurements |
| Adafruit Unified Sensor Library | Dependency required by the DHT library |
| Serial Monitor | Viewing sensor measurements |

---

### 4. Wiring Configuration

| DHT22 Connection | ESP32-S3 Connection |
|---|---|
| VCC / Red wire | 3.3 V |
| GND / Black wire | GND |
| DATA / Green wire | GPIO7 |

The DHT22 was powered using the ESP32-S3 3.3 V supply. The sensor data line was connected to GPIO7.

---

### 5. Firmware

```cpp
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  Serial.println("DHT22 test started");
}

void loop() {
  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT22");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}
```

---

### 6. Test Procedure

1. The ESP32-S3 was disconnected from the computer before wiring.
2. The DHT22 power, ground, and data wires were connected to the ESP32-S3.
3. The required Arduino libraries were installed.
4. The ESP32-S3 board and the correct serial port were selected in Arduino IDE.
5. The firmware was compiled and uploaded.
6. Serial Monitor was opened at 115200 baud.
7. Temperature and humidity measurements were observed.
8. The sensor was touched and exposed to breath to verify that the measurements reacted to environmental changes.

---

### 7. Results

The firmware compiled and uploaded successfully.

The DHT22 transmitted temperature and humidity measurements to the ESP32-S3. The ESP32-S3 displayed the readings through Serial Monitor approximately every two seconds.

Example output:

```text
DHT22 test started
Temperature: 24.60 °C | Humidity: 48.20 %
Temperature: 24.70 °C | Humidity: 48.10 %
Temperature: 24.60 °C | Humidity: 48.30 %
```

The exact measured values varied according to the surrounding environmental conditions.

---

### 8. Problem Encountered

After the firmware upload, no output initially appeared in Serial Monitor.

The Arduino IDE had previously referenced an outdated serial port. The ESP32-S3 also needed to be connected through the USB-UART interface for the selected serial configuration.

---

### 9. Solution

The following corrective actions were taken:

1. The active USB serial port was selected in Arduino IDE.
2. The ESP32-S3 was connected through the USB-UART connector.
3. Serial Monitor was configured to use 115200 baud.
4. The firmware was uploaded again.
5. The ESP32-S3 reset button was pressed after opening Serial Monitor.

After these corrections, the sensor output appeared successfully.

---

### 10. Observations

- The DHT22 produced stable environmental measurements.
- Temperature increased when the sensor was touched.
- Humidity changed when moist breath was directed toward the sensor.
- The DHT22 should not be sampled too frequently.
- A two-second sampling interval was used for the initial verification.
- USB-UART communication was required for reliable Serial Monitor output with the current board configuration.
- The current program uses `delay()`, which blocks the processor during the waiting period.

---

### 11. Technical Interpretation

The DHT22 performs the physical measurement of temperature and humidity.

The ESP32-S3 does not directly measure these environmental properties. Its role is to:

- request or receive data from the sensor;
- interpret the digital response;
- store the readings in variables;
- transmit the values through serial communication.

The data flow during this experiment was:

```text
Environment
    ↓
DHT22
    ↓
Digital sensor data
    ↓
ESP32-S3
    ↓
USB-UART serial communication
    ↓
Computer Serial Monitor
```

---

### 12. Conclusion

The experiment successfully verified the integration of the DHT22 sensor with the ESP32-S3.

The sensor was able to measure temperature and humidity, while the ESP32-S3 received and displayed the measurements correctly. This confirms that the environmental sensing component can be included in the proposed IoT inventory monitoring system.

The next software improvement will replace the blocking `delay()` approach with timing based on `millis()`. This will allow the ESP32-S3 to perform additional tasks while waiting between sensor measurements.

---

### 13. Next Action

- Rewrite the DHT22 firmware using non-blocking timing.
- Verify that measurements are still produced every two seconds.
- Prepare the firmware structure for multiple sensors.
- Begin independent testing of the HX711 and load cell.

---

## Experiment Summary

| Item | Result |
|---|---|
| ESP32-S3 firmware upload | Passed |
| DHT22 communication | Passed |
| Temperature measurement | Passed |
| Humidity measurement | Passed |
| USB-UART serial output | Passed |
| Independent sensor verification | Completed |
| Multi-sensor integration | Not started |

---

---

## Experiment 02 — Non-Blocking DHT22 Data Acquisition

### Date

19 July 2026

### Status

✅ Completed

---

### 1. Objective

The objective of this experiment was to replace the blocking `delay()` function with non-blocking timing based on `millis()`.

This change allows the ESP32-S3 to continue executing other tasks while waiting for the next DHT22 measurement.

---

### 2. Background

The original DHT22 program used:

```cpp
delay(2000);
```

This pauses the execution of the entire program for two seconds.

Although this approach is acceptable for a basic sensor test, it is unsuitable for a multi-sensor IoT system because the ESP32-S3 may later need to perform several tasks simultaneously, including:

- reading additional sensors;
- checking RFID or NFC input;
- maintaining Wi-Fi connectivity;
- publishing MQTT messages;
- storing data;
- responding to system events.

The `millis()` function provides the number of milliseconds elapsed since the ESP32-S3 started running. It can therefore be used to determine when a task should run without stopping the processor.

---

### 3. Firmware

```cpp
#include <DHT.h>

#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMeasurementTime = 0;
const unsigned long measurementInterval = 2000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  Serial.println("Non-blocking DHT22 test started");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - previousMeasurementTime >= measurementInterval) {
    previousMeasurementTime = currentTime;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT22");
      return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Other sensor and communication tasks can run here.
}
```

---

### 4. Test Procedure

1. A new Arduino sketch named `03_DHT22_Non_Blocking` was created.
2. The blocking `delay(2000)` statement was removed from the main loop.
3. Timing variables were defined using the `unsigned long` data type.
4. The current system time was read using `millis()`.
5. The DHT22 was read only when the two-second measurement interval had elapsed.
6. The firmware was compiled and uploaded to the ESP32-S3.
7. Serial Monitor was opened at 115200 baud.
8. The timing and sensor output were observed.

---

### 5. Results

The updated firmware operated successfully.

Temperature and humidity readings continued to appear approximately every two seconds, while the main loop remained active between measurements.

Example output:

```text
Non-blocking DHT22 test started
Temperature: 24.60 °C | Humidity: 48.20 %
Temperature: 24.70 °C | Humidity: 48.10 %
Temperature: 24.60 °C | Humidity: 48.30 %
```

---

### 6. Technical Explanation

The following condition controls the sensor timing:

```cpp
if (currentTime - previousMeasurementTime >= measurementInterval)
```

The variables represent:

| Variable | Meaning |
|---|---|
| `currentTime` | Current ESP32-S3 operating time in milliseconds |
| `previousMeasurementTime` | Time when the previous sensor reading occurred |
| `measurementInterval` | Required delay between readings |

When the difference between the current time and previous reading time reaches 2000 milliseconds, the sensor is read again.

The subtraction-based timing method is preferred because it continues to work correctly when the value returned by `millis()` eventually overflows.

---

### 7. Comparison

| Feature | `delay()` Method | `millis()` Method |
|---|---|---|
| Easy to understand | Yes | Moderate |
| Stops program execution | Yes | No |
| Suitable for one simple task | Yes | Yes |
| Suitable for multiple sensors | No | Yes |
| Suitable for Wi-Fi and MQTT | Limited | Yes |
| Recommended for final system | No | Yes |

---

### 8. Observations

- The DHT22 continued producing stable readings.
- The two-second measurement interval was maintained.
- The ESP32-S3 main loop continued running between sensor measurements.
- The firmware is now better prepared for multi-sensor integration.
- A short startup delay remains in `setup()`, but this does not affect continuous system operation.

---

### 9. Conclusion

The experiment successfully replaced blocking timing with a non-blocking scheduling method based on `millis()`.

This implementation is more appropriate for the proposed IoT inventory management system because it allows the ESP32-S3 to manage multiple sensors and communication tasks within the same program.

---

### 10. Next Action

- Preserve the working DHT22 code as the environmental sensing module.
- Assign GPIO pins for the HX711.
- Test the HX711 and load cell independently.
- Keep the DHT22 disconnected during the first HX711 test to reduce debugging complexity.

---

## Experiment 02 Summary

| Item | Result |
|---|---|
| Firmware compilation | Passed |
| Firmware upload | Passed |
| Non-blocking timing | Passed |
| Two-second sampling interval | Passed |
| Temperature measurement | Passed |
| Humidity measurement | Passed |
| Readiness for multi-sensor integration | Improved |

# Lab Experiment #2 – I²C Communication Test (Ultrasonic Sensor)

## Objective
Verify that the ESP32-S3 can detect the M5Stack Ultrasonic I²C sensor on the I²C bus.

## Hardware
- ESP32-S3
- M5Stack Ultrasonic I²C Sensor

## Wiring

ESP32 GPIO8  → SDA
ESP32 GPIO9  → SCL
ESP32 5V     → 5V
ESP32 GND    → GND

## Test Method

An I²C scanner sketch was uploaded to the ESP32. The program scanned all valid I²C addresses from 0x01 to 0x7F and reported any responding devices.

## Result

Detected device:

Address: 0x57

## Conclusion

The ultrasonic sensor successfully responded on the I²C bus, confirming that the wiring, power supply, and communication interface are functioning correctly.

---

## Experiment 03 — M5Stack Ultrasonic I²C Sensor Integration

### Date

25 July 2026

### Status

✅ Completed

---

### 1. Objective

The objective of this experiment was to integrate the M5Stack Ultrasonic I²C sensor with the ESP32-S3 and verify accurate distance measurement.

The experiment also aimed to confirm:

- correct power and I²C wiring;
- successful detection of the sensor on the I²C bus;
- communication through SDA and SCL;
- correct interpretation of the three-byte distance response;
- conversion of the sensor output into centimetres.

---

### 2. Hardware Used

| Component | Purpose |
|---|---|
| ESP32-S3 DevKitC-1 | Main microcontroller |
| M5Stack Ultrasonic I²C Sensor | Distance and fill-level measurement |
| Breadboard | Circuit prototyping |
| Jumper wires | Electrical connections |
| Grove sensor cable | Sensor connection |
| USB-C cable | Programming and serial communication |

---

### 3. Wiring Configuration

| Ultrasonic Sensor | ESP32-S3 |
|---|---|
| SDA | GPIO8 |
| SCL | GPIO9 |
| 5V | 5V |
| GND | GND |

The ultrasonic sensor communicates through the I²C protocol.

---

### 4. I²C Detection Test

Before attempting distance measurement, an I²C scanner was used to verify communication.

The ESP32-S3 successfully detected the sensor at:

```text
0x57
```

This confirmed that:

- the sensor was powered;
- SDA and SCL were connected correctly;
- the device was responding on the I²C bus.

---

### 5. Distance Measurement Firmware

```cpp
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
```

---

### 6. Test Procedure

1. The ultrasonic sensor was connected to the ESP32-S3.
2. SDA was connected to GPIO8.
3. SCL was connected to GPIO9.
4. The sensor was powered using 5 V and GND.
5. An I²C scanner was uploaded.
6. The sensor was detected at address `0x57`.
7. A distance-reading sketch was uploaded.
8. An object was moved closer to and farther from the sensor.
9. The displayed measurements were compared with the approximate physical distance.

---

### 7. Initial Problem

The first program displayed values such as:

```text
Distance: 744.1 cm
```

This exceeded the sensor’s valid measurement range and was clearly inaccurate.

---

### 8. Cause

The sensor data was decoded correctly, but the result was interpreted using the wrong unit.

The three-byte response produced a value in millimetres after division by 1000. The first program incorrectly displayed this value as centimetres.

For example:

```text
744.1 mm = 74.41 cm
```

---

### 9. Solution

The firmware was corrected by separating the millimetre and centimetre calculations:

```cpp
float distanceMm = rawData / 1000.0;
float distanceCm = distanceMm / 10.0;
```

A valid measurement range check was also added:

```cpp
if (distanceMm >= 20.0 && distanceMm <= 4500.0)
```

---

### 10. Results

After correcting the unit conversion, the sensor produced realistic and responsive distance values.

Example output:

```text
Ultrasonic distance test started
Distance: 74.4 cm
Distance: 53.8 cm
Distance: 31.2 cm
Distance: 18.6 cm
```

The readings changed consistently when an object was moved closer to or farther from the sensor.

---

### 11. Technical Interpretation

The sensor performs the ultrasonic measurement internally.

The measurement process is:

```text
Ultrasonic pulse transmitted
          ↓
Pulse travels toward object
          ↓
Object reflects sound wave
          ↓
Sensor receives echo
          ↓
Internal electronics calculate distance
          ↓
ESP32 reads result through I²C
```

Unlike simpler ultrasonic modules, the ESP32-S3 does not directly measure the echo duration. The sensor calculates the distance and returns the result as digital data.

---

### 12. Application in Inventory Management

The ultrasonic sensor can be used to estimate the fill level of a storage container.

When mounted above a container:

- a short measured distance indicates a high stock level;
- a long measured distance indicates a low stock level;
- the empty container distance can be used as a reference point.

The fill percentage can later be calculated using:

```text
Fill percentage =
(Container depth − Measured distance)
÷ Container depth
× 100
```

This method can complement the load cell by providing both physical fill-level and weight information.

---

### 13. Observations

- The sensor responded reliably at I²C address `0x57`.
- GPIO8 and GPIO9 worked correctly as SDA and SCL.
- The sensor required correct unit conversion.
- Out-of-range validation improved measurement reliability.
- The sensor readings reacted quickly to object movement.
- Soft, angled, narrow, or irregular surfaces may produce less stable echoes.
- The sensor must be mounted perpendicular to the measured surface for better results.

---

### 14. Conclusion

The M5Stack Ultrasonic I²C sensor was successfully integrated with the ESP32-S3.

The experiment confirmed reliable I²C communication and accurate distance measurement after correcting the unit conversion. The sensor is suitable for use as a contactless inventory-level sensing component in the proposed system.

---

### 15. Next Action

- Replace blocking delays with non-blocking timing.
- Create a reusable ultrasonic sensor function.
- Integrate DHT22 and ultrasonic sensing in one firmware sketch.
- Test both sensors operating simultaneously.
- Calculate container fill percentage using a defined container depth.

---

## Experiment 03 Summary

| Item | Result |
|---|---|
| Sensor power | Passed |
| I²C communication | Passed |
| I²C address detection | `0x57` |
| Distance measurement | Passed |
| Unit conversion | Corrected |
| Valid-range filtering | Implemented |
| Inventory-level suitability | Confirmed |


# Experiment 04 — Multi-Sensor Integration

## Objective

Integrate multiple sensors on the ESP32-S3 and verify that they operate simultaneously without communication conflicts.

---

## Sensors Used

| Sensor | Communication | GPIO |
|---------|---------------|------|
| DHT22 | Single-wire Digital | GPIO7 |
| Ultrasonic I²C | SDA/SCL | GPIO8 / GPIO9 |

---

## Wiring

### DHT22

- VCC → 3.3V
- GND → GND
- DATA → GPIO7

### Ultrasonic

- SDA → GPIO8
- SCL → GPIO9
- VCC → 5V
- GND → GND

---

## Software Design

The firmware was divided into two independent functions:

- readDHT22()
- readUltrasonic()

Each sensor was executed using separate timers based on millis().

This allows the ESP32 to read multiple sensors without blocking the entire program.

---

## Results

The ESP32 successfully collected:

- Temperature
- Humidity
- Distance

All sensor readings were displayed correctly in the Serial Monitor.

No I²C conflicts or GPIO conflicts were observed.

---

## Learning Outcomes

This experiment demonstrated:

- multiple sensors can run simultaneously on the ESP32-S3;
- different communication protocols (Single-wire and I²C) can coexist;
- modular programming makes firmware easier to maintain;
- timer-based scheduling is preferable to placing all code inside loop().

---

## Conclusion

The ESP32-S3 successfully operated multiple sensors simultaneously, forming the foundation of the IoT inventory monitoring system.


## Version History

| Version | Date | Description |
|---|---|---|
| 1.0 | 18 July 2026 | Engineering lab book created and Experiment 01 documented |
| 1.1 | 19 July 2026 | Added Experiment 02: non-blocking DHT22 data acquisition using `millis()` |
| 1.2 | 25 July 2026 | Added Experiment 03: ultrasonic I²C detection and distance measurement |


## HX711 Load Cell Amplifier Communication Test

### Objective

The objective of this test was to verify communication between the ESP32-S3 and the HX711 load cell amplifier before proceeding with load cell measurement and calibration.

### Hardware Used

- ESP32-S3 development board
- HX711 load cell amplifier
- Breadboard
- Jumper wires
- USB connection to MacBook

### Wiring

| HX711 Pin | ESP32-S3 Connection |
|-----------|----------------------|
| VDD | 3.3V |
| VCC | 5V |
| GND | GND |
| DAT | GPIO4 |
| CLK | GPIO5 |

### Procedure

The HX711 was connected to the ESP32-S3 to test communication between the two devices.

During the initial wiring attempt, the ESP32-S3 did not start normally and became unusually warm. The USB connection was immediately disconnected and the HX711 circuit was removed from the ESP32-S3.

The ESP32-S3 was then tested independently without the HX711. The board powered normally and its USB-to-serial interface was successfully detected by the MacBook as:

/dev/cu.usbserial-1130

This confirmed that the ESP32-S3 itself was still functioning correctly.

The HX711 power connections were then checked and the circuit was rebuilt carefully. The final power and communication connections were:

- HX711 VDD → ESP32-S3 3.3V
- HX711 VCC → ESP32-S3 5V
- HX711 GND → ESP32-S3 GND
- HX711 DAT → ESP32-S3 GPIO4
- HX711 CLK → ESP32-S3 GPIO5

After reconnecting the circuit, the ESP32-S3 powered normally without abnormal heating.

A dedicated Arduino test sketch was created:

`09_HX711_Raw_Test.ino`

The sketch initialized the HX711 and continuously retrieved raw ADC readings from the module.

### Results

Communication between the ESP32-S3 and HX711 was successfully established.

Example raw readings observed in the Arduino Serial Monitor were:

- -1650
- -1669
- -1665
- -1664
- -1651
- -1646
- -1673
- -1654
- -1649
- -1667

The readings remained approximately around -1650 to -1670 with small fluctuations.

This confirmed that the ESP32-S3 was successfully receiving raw data from the HX711.

### Troubleshooting

An important issue occurred during the initial test: the ESP32-S3 became unusually warm and failed to operate normally when the HX711 circuit was connected.

To troubleshoot the problem:

1. The HX711 was completely disconnected.
2. The ESP32-S3 was tested independently.
3. USB serial detection was verified.
4. The HX711 power connections were checked.
5. The HX711 was reconnected gradually.
6. Communication pins DAT and CLK were added after confirming stable power operation.

After correcting and verifying the connections, the overheating problem no longer occurred and HX711 communication worked normally.

### Conclusion

The HX711 load cell amplifier was successfully interfaced with the ESP32-S3.

The test confirmed that:

- The HX711 could communicate with the ESP32-S3.
- GPIO4 successfully functioned as the HX711 DAT connection.
- GPIO5 successfully functioned as the HX711 CLK connection.
- Raw ADC values could be received continuously.
- The ESP32-S3 and HX711 operated normally after the wiring issue was resolved.

The actual load cell measurement and calibration were not completed during this experiment and were postponed for a later stage of the project.

### Status

**HX711 communication: Successfully verified**

**Load cell measurement and calibration: Postponed**


## RC522 RFID Reader Communication and Tag Detection Test

### Objective

The objective of this test was to verify communication between the ESP32-S3 and the MFRC522 RFID reader and to confirm that an RFID tag could be detected and its UID read successfully.

### Hardware Used

- ESP32-S3 development board
- RFID-RC522 module
- RFID key fob/tag
- Breadboard
- Jumper wires
- USB connection to MacBook

### Wiring

| RC522 Pin | ESP32-S3 Connection |
|-----------|----------------------|
| SDA / SS | GPIO10 |
| SCK | GPIO12 |
| MOSI | GPIO11 |
| MISO | GPIO13 |
| IRQ | Not connected |
| GND | GND |
| RST | GPIO14 |
| 3.3V | 3.3V |

### Important Power Note

The RC522 module operates at 3.3V logic and power.

The RC522 3.3V pin was connected only to the ESP32-S3 3.3V pin.

The RC522 module was not connected to 5V.

### Procedure

The RC522 was connected to the ESP32-S3 using SPI communication.

A dedicated Arduino test sketch was created:

`10_RC522_Test.ino`

The MFRC522 Arduino library was used for communication with the RFID reader.

During the first compilation attempt, two problems were encountered.

The first error was:

`no matching function for call to 'SPIClass::begin(int, int, int, int)'`

The compiler output showed that the Arduino AVR SPI library was being used instead of the ESP32 SPI library. The board selection in Arduino IDE was corrected to:

`ESP32S3 Dev Module`

A second compilation error occurred because multiple `setup()` and `loop()` functions were present in the sketch. The old code was removed and replaced with a single complete RC522 test program.

After correcting these issues, the sketch compiled and uploaded successfully.

The program initialized SPI communication using the selected ESP32-S3 pins and read the RC522 version register.



### RC522 Detection Result

The Arduino Serial Monitor displayed:

`RC522 Version Register: 0x92`

followed by:

`SUCCESS: RC522 detected.`

This confirmed successful SPI communication between the ESP32-S3 and the RC522 reader.

### RFID Tag Detection

After successful reader detection, an RFID tag was placed near the antenna of the RC522 module.

The Serial Monitor displayed:

`RFID UID: 23:31:7F:33`

This confirmed that the RC522 successfully detected the RFID tag and read its unique identifier.

### Results

The following functions were verified successfully:

- SPI communication between ESP32-S3 and RC522
- RC522 initialization
- RC522 version register reading
- RFID tag detection
- RFID UID reading

The detected RFID tag UID was:

`23:31:7F:33`

### Troubleshooting

Two software-related issues were identified during the test.

#### Incorrect Board Selection

The Arduino IDE initially compiled the sketch using the AVR SPI library.

The compiler path showed:

`arduino/hardware/avr/1.8.8/libraries/SPI`

The correct ESP32-S3 board was then selected:

`ESP32S3 Dev Module`

This enabled the ESP32-specific SPI initialization function.

#### Duplicate setup() and loop() Functions

The sketch initially contained more than one `setup()` and `loop()` function because new code had been appended to existing code.

The sketch was cleared and replaced with a single complete test program.

After these corrections, compilation and upload were successful.

### Conclusion

The RFID-RC522 reader was successfully interfaced with the ESP32-S3.

The test confirmed that:

- The ESP32-S3 could communicate with the RC522 using SPI.
- The RC522 responded correctly with version register value `0x92`.
- The RC522 successfully detected an RFID tag.
- The tag UID was successfully read as `23:31:7F:33`.
- The selected SPI pin configuration worked correctly.

The RFID reader is therefore ready for integration into the inventory management system.

The next planned step is to publish RFID scan events through MQTT so that RFID data can be received by the Raspberry Pi.

### Status

**RC522 communication: Successfully verified**

**RFID tag detection: Successfully verified**

**Detected test UID: `23:31:7F:33`**

**Next step: RFID event publishing through MQTT**




## RC522 RFID to MQTT Integration Test

### Objective

The objective of this test was to integrate the RC522 RFID reader with the existing MQTT communication system and verify that an RFID scan event generated by the ESP32-S3 could be transmitted through the MQTT broker and received successfully by the Raspberry Pi 5.

This test extended the previous standalone RC522 hardware test into a complete end-to-end IoT communication pipeline.

### System Architecture

The data flow used in this test was:

RFID Tag  
→ RC522 RFID Reader  
→ ESP32-S3  
→ JSON Message  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5

The MQTT broker used was:

`broker.hivemq.com`

The MQTT topic used for RFID events was:

`savonia/mahfuz/iot_thesis/rfid`

### Hardware Used

- ESP32-S3 development board
- RFID-RC522 module
- RFID key fob/tag
- Raspberry Pi 5
- Breadboard
- Jumper wires
- Wi-Fi network
- MacBook for Arduino IDE and SSH access

### RC522 Wiring

| RC522 Pin | ESP32-S3 Connection |
|-----------|----------------------|
| SDA / SS | GPIO10 |
| SCK | GPIO12 |
| MOSI | GPIO11 |
| MISO | GPIO13 |
| IRQ | Not connected |
| GND | GND |
| RST | GPIO14 |
| 3.3V | 3.3V |

### Software Used

- Arduino IDE
- MFRC522 Arduino library
- PubSubClient MQTT library
- WiFi library for ESP32
- Mosquitto command-line client on Raspberry Pi

### Arduino Test Sketch

A new Arduino sketch was created:

`11_RC522_MQTT_Test.ino`

The sketch performed the following functions:

1. Initialized the RC522 RFID reader.
2. Verified RC522 communication by reading the version register.
3. Connected the ESP32-S3 to Wi-Fi.
4. Connected the ESP32-S3 to the HiveMQ MQTT broker.
5. Waited for an RFID tag to be scanned.
6. Read the RFID UID.
7. Created a structured JSON message.
8. Published the JSON message to the RFID MQTT topic.

### RC522 Initialization Result

The Arduino Serial Monitor displayed:

`RC522 Version: 0x92`

This confirmed that communication between the ESP32-S3 and RC522 was functioning correctly.

### Wi-Fi and MQTT Connection Result

The Serial Monitor showed:

`Wi-Fi connected!`

The ESP32-S3 received the IP address:

`172.20.10.13`

The MQTT connection was then established successfully:

`Connecting to MQTT...connected!`

The system then displayed:

`System ready.`

`Scan an RFID tag...`

### RFID Scan Result

The same RFID test tag used in the previous standalone RFID test was scanned.

The detected UID was:

`23:31:7F:33`

The Serial Monitor displayed:

`RFID detected: 23:31:7F:33`

followed by:

`MQTT publish successful:`

The ESP32-S3 created and published a JSON message containing the RFID event data.

The message structure was:

```json
{
  "device_id": "esp32_s3_01",
  "event": "rfid_scan",
  "uid": "23:31:7F:33",
  "uptime_ms": 294806
}



## Raspberry Pi Python RFID MQTT Subscriber Test

### Objective

The objective of this test was to replace the command-line MQTT subscription test with a Python-based MQTT subscriber running on the Raspberry Pi 5.

The goal was to verify that the Raspberry Pi could receive RFID scan events from the ESP32-S3, parse the JSON payload, and extract individual RFID event fields programmatically.

### System Architecture

The data flow used in this test was:

RFID Tag  
→ RC522 RFID Reader  
→ ESP32-S3  
→ JSON Message  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5  
→ Python MQTT Subscriber

The MQTT broker used was:

`broker.hivemq.com`

The MQTT topic used was:

`savonia/mahfuz/iot_thesis/rfid`

### Raspberry Pi Environment

The Raspberry Pi project directory was:

`~/IoT_Thesis/Software/RaspberryPi`

A Python virtual environment was activated using:

`source venv/bin/activate`

The Python MQTT client library used was:

`paho-mqtt`

### Python Subscriber Program

A new Python program was created:

`02_RFID_MQTT_Subscriber.py`

The program performed the following functions:

1. Connected to the HiveMQ MQTT broker.
2. Subscribed to the RFID MQTT topic.
3. Waited for incoming RFID events.
4. Decoded the MQTT payload.
5. Parsed the JSON message.
6. Extracted individual data fields.
7. Printed the parsed RFID information to the Raspberry Pi terminal.

### MQTT Connection Result

The Python program successfully connected to the broker.

The Raspberry Pi terminal displayed:

`Connecting to MQTT broker...`

`Connected to MQTT broker`

`Subscribed to: savonia/mahfuz/iot_thesis/rfid`

This confirmed that the Python program was connected and actively listening for RFID events.

### RFID Event Test

The RFID tag was scanned using the RC522 reader connected to the ESP32-S3.

The ESP32-S3 detected the tag and published the RFID event through MQTT.

The Python subscriber on the Raspberry Pi received and processed the message successfully.

The terminal displayed:

```text
--- RFID Event Received ---
Device ID : esp32_s3_01
Event     : rfid_scan
RFID UID  : 23:31:7F:33
Uptime ms : 3265680
---------------------------



## Raspberry Pi RFID Inventory CSV Logging Test

### Objective

The objective of this test was to extend the Raspberry Pi RFID processing system so that each RFID scan event could be stored permanently in a CSV file.

Previous tests had already confirmed that the Raspberry Pi could:

- receive RFID scan events through MQTT,
- parse the incoming JSON message,
- extract the RFID UID,
- and map the UID to a known inventory item.

This test added persistent event logging so that RFID inventory activity could be saved for later analysis.

### System Architecture

The tested data flow was:

RFID Tag  
→ RC522 RFID Reader  
→ ESP32-S3  
→ JSON Message  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5  
→ Python RFID Processing  
→ Inventory Item Recognition  
→ CSV File Storage

### Raspberry Pi Project Directory

The program was executed from:

`~/IoT_Thesis/Software/RaspberryPi`

The Python virtual environment was activated using:

`source venv/bin/activate`

### Python Program

The CSV logging program used was:

`02_Save_to_CSV.py`

The program performed the following functions:

1. Connected to the HiveMQ MQTT broker.
2. Subscribed to the RFID MQTT topic.
3. Received the RFID JSON event.
4. Parsed the incoming JSON message.
5. Extracted the RFID UID.
6. Compared the UID with the local inventory mapping.
7. Identified the corresponding inventory item.
8. Generated a timestamp.
9. Saved the RFID event as a new row in a CSV file.

### MQTT Configuration

The MQTT broker was:

`broker.hivemq.com`

The RFID topic was:

`savonia/mahfuz/iot_thesis/rfid`

### Inventory Mapping

The test RFID UID was mapped to the following inventory item:

| Field | Value |
|---|---|
| RFID UID | `23:31:7F:33` |
| Product ID | `Product_001` |
| Name | `Test Inventory Item` |
| Category | `Prototype` |
| Status | `Known RFID tag` |

### CSV File

The program created and used the following file:

`rfid_inventory_log.csv`

The CSV structure contained these columns:

```text
timestamp
device_id
event
rfid_uid
product_id
name
category
status
uptime_ms




## HX711 Load Cell Connection and Calibration Test

### Objective

The objective of this experiment was to connect the physical load cell to the previously verified HX711 amplifier, confirm that the load cell responded to applied force, and calibrate the system so that raw HX711 values could be converted into weight measurements in grams.

### Hardware Used

- ESP32-S3 development board
- HX711 load cell amplifier
- Four-wire load cell
- Breadboard
- Jumper wires
- MacBook running Arduino IDE
- 1000 g reference weight
- Additional packaged item for practical measurement testing

### Load Cell Wiring

The four-wire load cell was soldered to the HX711 amplifier.

| Load Cell Wire | HX711 Connection |
|----------------|------------------|
| Red | RED |
| Black | BLK |
| White | WHT |
| Green | GRN |
| No connection | YLW |

The soldered connections were visually inspected before testing.

### HX711 to ESP32-S3 Wiring

| HX711 Pin | ESP32-S3 |
|-----------|-----------|
| VDD | 3.3V |
| VCC | 5V |
| GND | GND |
| DAT | GPIO4 |
| CLK | GPIO5 |

### Initial Functional Test

The existing raw HX711 test program was used first to confirm that the newly soldered load cell was electrically functional.

Without significant load, the raw HX711 readings stabilized at approximately:

`-23,300 to -23,500`

When pressure was applied to the load cell, the readings changed substantially, including values such as:

`-703878`

`-722599`

`-729274`

`-683226`

`-643233`

When the force was removed, the readings returned close to the original unloaded value.

This confirmed that the complete measurement chain was functioning:

Load Cell  
→ HX711  
→ ESP32-S3

### Calibration Procedure

A known reference weight of:

`1000 g`

was used for calibration.

The approximate raw value without load was:

`-23,500`

The raw value with the 1000 g reference weight applied was approximately:

`-231,950`

The approximate change in raw HX711 counts was therefore:

`-231950 - (-23500) = -208450`

The initial calibration factor was calculated as:

`-208450 / 1000 = -208.45 counts/g`

The following calibration factor was therefore used in the Arduino program:

`-208.45`

### Tare Test

Before applying the reference weight, the load cell was unloaded and the HX711 tare function was executed.

After taring, the measured values were approximately:

`-0.1 g`

`0.0 g`

`0.1 g`

This confirmed that the zero offset was successfully removed.

### 1000 g Calibration Result

After taring, the 1000 g reference weight was placed on the load cell.

During placement, temporary transition readings were observed, including:

`798.5 g`

`573.4 g`

After the weight was positioned correctly and allowed to stabilize, the readings became:

`999.7 g`

`999.7 g`

`999.8 g`

`999.7 g`

`999.7 g`

`999.7 g`

`999.8 g`

The stabilized measurement was therefore approximately:

`999.7–999.8 g`

for a reference weight of:

`1000 g`

The calibration factor of:

`-208.45`

was retained.

### Practical Object Test

A packaged food item was also placed on the load cell as a practical measurement test.

The measured values stabilized at approximately:

`409.7–409.9 g`

The package label showed:

`380 g / 205 g`

However, these label values were not treated as an accuracy reference because the package did not provide a verified total gross weight including packaging.

The test was therefore used only to confirm that the calibrated system produced stable measurements for a different real-world object.

### Observations

The load cell demonstrated very stable measurements once the applied weight was stationary.

At zero load, measurements remained close to 0 g.

At the 1000 g calibration point, the measurement remained close to the reference value.

The different packaged object also produced consistent readings around 409.8 g.

Temporary intermediate readings occurred while objects were being placed or repositioned. These were expected because the load cell was experiencing changing mechanical force during movement.

### Results

The following functions were successfully verified:

- Load cell solder connections
- Load cell response to applied force
- HX711 raw ADC measurement
- Zero-load stability
- HX711 tare function
- Conversion of raw readings to grams
- Calibration using a 1000 g reference weight
- Stable measurement of the 1000 g reference
- Stable measurement of an additional real-world object

### Conclusion

The load cell and HX711 amplifier were successfully calibrated with the ESP32-S3.

Using a calibration factor of:

`-208.45`

the system measured a 1000 g reference weight at approximately:

`999.7–999.8 g`

after stabilization.

The load cell therefore demonstrated successful weight measurement and good repeatability at the calibration point.

Further validation using several independent known weights is still required before making a formal claim about the overall accuracy, linearity, and measurement error of the system.

### Status

**Load cell wiring: Successfully verified**

**HX711 communication: Successfully verified**

**Load response: Successfully verified**

**Tare function: Successfully verified**

**1000 g calibration: Successfully completed**

**Current calibration factor: `-208.45`**

**Multi-point accuracy validation: Pending**

### Next Step

Perform multi-point validation using several known reference weights, for example:

250 g  
500 g  
750 g  
1000 g

For each reference weight, record the measured value and calculate:

- Absolute error
- Percentage error
- Repeatability
- Linearity across the measurement range


### HX711 Load Cell Calibration Update

The physical load cell was subsequently connected and calibrated successfully.

The unloaded raw HX711 reading was approximately `-23,500`, while a known 1000 g reference weight produced approximately `-231,950` raw counts.

An initial calibration factor of:

`-208.45 counts/g`

was calculated and applied.

After performing a tare operation, the unloaded system remained approximately within `-0.1 g` to `0.1 g`.

With the 1000 g reference weight applied, stabilized measurements were approximately:

`999.7–999.8 g`

This confirms successful conversion of HX711 raw readings into weight measurements.

A second practical object produced stable readings of approximately `409.7–409.9 g`, demonstrating repeatable measurement with a different load.

Formal multi-point validation is still required before reporting overall measurement accuracy or linearity.

**Status:** Load cell connected and calibrated successfully. Multi-point validation pending.

## HX711 Load Cell Multi-Object Validation Test

### Objective

The objective of this experiment was to further evaluate the calibrated HX711 load-cell system using several different physical loads.

The previous experiment established a calibration factor of:

`-208.45`

using a known 1000 g reference weight.

This experiment focused on:

- repeatability of the measurements,
- stability under constant load,
- return to zero after removing a load,
- and practical measurement of objects with different masses.

### Test Setup

The same hardware configuration and calibration factor from the previous experiment were used.

#### HX711 to ESP32-S3 Connections

| HX711 Pin | ESP32-S3 |
|-----------|-----------|
| VDD | 3.3V |
| VCC | 5V |
| GND | GND |
| DAT | GPIO4 |
| CLK | GPIO5 |

Calibration factor:

`-208.45`

The load cell was tared before measurements were taken.

---

### Test 1 – 1000 g Reference Weight

A known 1000 g reference weight had previously been used for calibration.

After stabilization, the system measured approximately:

`999.7–999.8 g`

This confirmed that the calibration factor produced a measurement very close to the known reference value at the calibration point.

---

### Test 2 – 750 mL Packaged Product

A 750 mL packaged cleaning product was placed on the load cell.

The initial value changed while the bottle was being positioned, including a temporary reading of:

`773.2 g`

After the object stabilized, the measurements were:

`798.4 g`

`798.3 g`

`798.4 g`

`798.3 g`

`798.4 g`

`798.4 g`

`798.4 g`

`798.5 g`

The stable measured mass was therefore approximately:

`798.4 g`

The observed stable range was approximately:

`798.3–798.5 g`

This represents a measurement spread of approximately:

`0.2 g`

The printed 750 mL value represents liquid volume rather than verified total package mass. Therefore, this measurement was used to evaluate stability and repeatability rather than formal accuracy.

---

### Zero Return Test

After removing the 750 mL bottle, the load cell readings returned to approximately:

`0.3 g`

`0.3 g`

`0.5 g`

`0.4 g`

This demonstrated that the system returned close to its original zero point after the load was removed.

---

### Test 3 – 500 mL Packaged Product

A second packaged product with a labeled volume of 500 mL was placed on the load cell.

After stabilization, the measurements were:

`564.8 g`

`564.8 g`

`564.7 g`

`564.7 g`

`564.8 g`

`564.8 g`

`564.8 g`

`564.8 g`

`564.8 g`

`564.7 g`

`564.9 g`

The stable measured mass was therefore approximately:

`564.8 g`

The observed measurement range was:

`564.7–564.9 g`

The measurement spread was approximately:

`0.2 g`

As with the previous packaged product, the printed 500 mL value represents liquid volume rather than verified gross package mass. The measurement was therefore used primarily to evaluate sensor stability and repeatability.

---

### Measurement Summary

| Test Object | Measurement Result | Stable Range |
|-------------|-------------------|--------------|
| No load after tare | Approximately 0 g | Around ±0.1 g initially |
| 1000 g reference | Approximately 999.7–999.8 g | Very stable |
| 750 mL packaged product | Approximately 798.4 g | 798.3–798.5 g |
| No load after removal | Approximately 0.3–0.5 g | Close to zero |
| 500 mL packaged product | Approximately 564.8 g | 564.7–564.9 g |

### Observations

The load-cell system demonstrated strong measurement stability after each object was allowed to settle.

Both packaged products produced a stable measurement spread of approximately 0.2 g.

The system also returned close to zero after the 750 mL object was removed.

Temporary intermediate readings occurred while objects were being placed on the load cell. These readings were expected because the applied force was changing during positioning.

The stable measurements should therefore be evaluated only after the object is stationary.

### Results

The following behaviors were successfully verified:

- Stable zero measurement after tare
- Accurate measurement at the 1000 g calibration point
- Measurement of different physical loads
- Good measurement repeatability
- Low fluctuation under constant load
- Return close to zero after removing a load
- Consistent operation of the load cell, HX711, and ESP32-S3

### Important Limitation

Only the 1000 g weight was a known reference mass.

The 500 mL and 750 mL package labels describe liquid volume and do not provide a certified total gross mass including the container.

Therefore, these two objects should not be used to calculate formal sensor accuracy.

They were used as practical test objects to evaluate:

- stability,
- repeatability,
- load response,
- and zero return.

Formal accuracy and linearity evaluation should use several independently verified reference masses.

### Conclusion

The calibrated HX711 load-cell system successfully measured multiple physical loads and demonstrated good repeatability.

Using the calibration factor:

`-208.45`

the 1000 g reference was measured at approximately:

`999.7–999.8 g`

The two additional test objects produced stable measurements of approximately:

`798.4 g`

and:

`564.8 g`

The system also returned close to zero after load removal.

These results indicate that the load-cell subsystem is functioning reliably and is suitable for continued development within the IoT inventory management system.

Formal multi-point accuracy validation with verified reference weights remains recommended before reporting final accuracy specifications.

### Status

**1000 g calibration: Successfully completed**

**Multi-object measurement: Successfully completed**

**Measurement repeatability: Successfully demonstrated**

**Zero return: Successfully demonstrated**

**Calibration factor: `-208.45`**

**Formal multi-point accuracy validation: Pending**

### Next Step

The load-cell subsystem can now be considered functionally operational.

The next technical stages are:

- validation with additional known reference weights,
- integration of calibrated weight data into MQTT messages,
- and later combination with RFID and other inventory sensors.




## HX711 Load Cell MQTT Integration Test

### Objective

The objective of this experiment was to integrate the calibrated HX711 load-cell subsystem with the existing MQTT communication architecture.

The goal was to verify that a physical weight measured by the load cell could be:

- converted into grams by the ESP32-S3,
- encoded as structured JSON data,
- published through MQTT,
- and received successfully by the Raspberry Pi 5.

### System Architecture

The tested data flow was:

Physical Weight  
→ Load Cell  
→ HX711 Amplifier  
→ ESP32-S3  
→ Calibrated Weight in Grams  
→ JSON Message  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5

### Hardware Used

- ESP32-S3 development board
- HX711 load cell amplifier
- Four-wire load cell
- Breadboard
- Jumper wires
- Raspberry Pi 5
- 1000 g reference weight
- Wi-Fi network

### HX711 Wiring

| HX711 Pin | ESP32-S3 |
|-----------|-----------|
| VDD | 3.3V |
| VCC | 5V |
| GND | GND |
| DAT | GPIO4 |
| CLK | GPIO5 |

### Calibration Factor

The previously established calibration factor was retained:

`-208.45`

This factor had been calculated using a known 1000 g reference weight.

### Arduino Test Sketch

A new Arduino integration sketch was created:

`12_HX711_MQTT_Test.ino`

The sketch performed the following functions:

1. Initialized the HX711.
2. Applied the calibration factor.
3. Tared the load cell.
4. Connected the ESP32-S3 to Wi-Fi.
5. Connected to the MQTT broker.
6. Read calibrated weight values in grams.
7. Created a JSON payload.
8. Published the weight data through MQTT every few seconds.

### MQTT Configuration

The MQTT broker was:

`broker.hivemq.com`

The MQTT topic used for weight measurements was:

`savonia/mahfuz/iot_thesis/weight`

### ESP32-S3 Test Result

After startup, the load cell was kept empty during the tare operation.

The ESP32-S3 successfully connected to Wi-Fi and MQTT.

The Serial Monitor displayed values close to zero, for example:

`Weight: -0.1 g`

The ESP32-S3 successfully published the corresponding JSON message.

An example message structure was:

```json
{
  "device_id": "esp32_s3_01",
  "sensor": "load_cell",
  "weight_g": -0.1,
  "uptime_ms": 123456
}



## Raspberry Pi Python Weight MQTT Subscriber Test

### Objective

The objective of this experiment was to create a Python-based MQTT subscriber on the Raspberry Pi 5 for receiving and processing calibrated load-cell weight data.

The previous experiment had already verified that the ESP32-S3 could:

- measure physical weight using the load cell and HX711,
- convert the measurement into grams,
- encode the value in JSON format,
- and publish it through MQTT.

This experiment added a Python processing layer on the Raspberry Pi.

### System Architecture

The tested data flow was:

Physical Weight  
→ Load Cell  
→ HX711  
→ ESP32-S3  
→ Calibrated Weight in Grams  
→ JSON  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5  
→ Python MQTT Subscriber

### Raspberry Pi Environment

The Python subscriber was executed from:

`~/IoT_Thesis/Software/RaspberryPi`

The Python virtual environment was activated using:

`source venv/bin/activate`

The MQTT Python library used was:

`paho-mqtt`

### Python Program

A new Python program was created:

`03_Weight_MQTT_Subscriber.py`

The program performed the following functions:

1. Connected to the HiveMQ MQTT broker.
2. Subscribed to the weight MQTT topic.
3. Received incoming MQTT messages.
4. Decoded the JSON payload.
5. Extracted the device ID.
6. Extracted the sensor type.
7. Extracted the calibrated weight value.
8. Extracted the ESP32 uptime value.
9. Printed the parsed data to the Raspberry Pi terminal.

### MQTT Configuration

The MQTT broker was:

`broker.hivemq.com`

The MQTT topic was:

`savonia/mahfuz/iot_thesis/weight`

### Test Procedure

The ESP32-S3 was already running the calibrated HX711 MQTT program.

The Raspberry Pi Python subscriber was started using:

```bash
python3 03_Weight_MQTT_Subscriber.py



## ESP32-S3 Multi-Sensor Integration and Troubleshooting Test

### Objective

The objective of this experiment was to integrate all four sensing subsystems on a single ESP32-S3 and verify that they could operate simultaneously without GPIO conflicts, power problems, or communication interference.

The integrated subsystems were:

- DHT22 temperature and humidity sensor
- M5Stack Ultrasonic-I2C distance sensor
- HX711 load cell amplifier and load cell
- RC522 RFID reader

Wi-Fi and MQTT were intentionally excluded from this experiment.

The purpose was first to establish a reliable local hardware and sensor-integration baseline before adding network communication.

---

### Final ESP32-S3 Pin Configuration

| Device | Function | ESP32-S3 Pin |
|--------|----------|--------------|
| HX711 | DAT | GPIO4 |
| HX711 | CLK | GPIO5 |
| DHT22 | DATA | GPIO7 |
| Ultrasonic | SDA | GPIO8 |
| Ultrasonic | SCL | GPIO9 |
| RC522 | SDA / SS | GPIO10 |
| RC522 | MOSI | GPIO11 |
| RC522 | SCK | GPIO12 |
| RC522 | MISO | GPIO13 |
| RC522 | RST | GPIO14 |

### Power Configuration

| Device | Power |
|--------|-------|
| DHT22 | 3.3V |
| RC522 | 3.3V |
| Ultrasonic-I2C | 5V |
| HX711 VDD | 3.3V |
| HX711 VCC | 5V |
| All devices | Common GND |

### Ultrasonic Cable Color Mapping

The successfully verified ultrasonic wiring was:

| Wire Color | Function | ESP32-S3 |
|------------|----------|-----------|
| Black | GND | GND |
| Red | Power | 5V |
| Yellow | SDA | GPIO8 |
| White | SCL | GPIO9 |

The ultrasonic sensor used I2C address:

`0x57`

---

### Arduino Integration Program

A new Arduino sketch was created:

`13_All_Sensors_Test.ino`

The program initialized and read all four sensor subsystems.

The intended output included:

- temperature,
- humidity,
- distance,
- calibrated weight,
- and RFID UID when a tag was scanned.

The HX711 calibration factor remained:

`-208.45`

The load cell was kept empty during startup so that the system could perform a tare operation.

---

## Initial Combined Test

The first combined test was only partially successful.

The DHT22 produced valid measurements such as:

`Temperature: 22.3 C`

`Humidity: 59.9 %`

However, two problems appeared.

The ultrasonic sensor returned:

`Distance: READ ERROR`

The HX711 produced an unrealistic weight of approximately:

`-22397 g`

and later approximately:

`-22451 g`

These values indicated that the problem was not a normal calibration error.

The combined system was therefore not considered functional at this stage.

---

## Ultrasonic Sensor Troubleshooting

To isolate the ultrasonic problem, an I2C scanner was uploaded while the sensor remained connected.

The first scanner result was:

`No I2C devices found.`

This confirmed that the ESP32-S3 could not communicate electrically with the ultrasonic module.

The ultrasonic wiring was then checked and corrected.

The verified connections were:

- Black → GND
- Red → 5V
- Yellow → GPIO8 / SDA
- White → GPIO9 / SCL

The I2C scanner was executed again.

The ESP32-S3 then repeatedly detected:

`I2C device found at 0x57`

This confirmed that the ultrasonic communication problem had been caused by a wiring/contact issue rather than the software.

After restoring the combined program, the ultrasonic sensor successfully produced distance measurements.

Example:

`Distance: 60.2 cm`

---

## HX711 Troubleshooting

After correcting the ultrasonic sensor, the HX711 still produced unrealistic values around:

`-22420 g`

A fresh tare did not solve the problem.

An HX711-only isolation program was therefore uploaded while investigating whether the issue originated from software or hardware.

During one test, the output showed:

`ERROR: HX711 not detected!`

The other sensors were then disconnected temporarily and the HX711 wiring was checked.

The HX711 was tested independently and returned stable unloaded values such as:

`-0.1 g`

`-0.0 g`

`-0.2 g`

This confirmed that:

- the HX711 module was functional,
- the load cell was functional,
- the calibration factor remained valid,
- and the earlier abnormal readings were related to the combined wiring/contact configuration.

---

## Sequential Sensor Reconnection

To identify possible electrical interference, the other sensors were reconnected one at a time while the HX711 isolation program continued running.

### HX711 + DHT22

After reconnecting the DHT22, the HX711 remained stable near zero.

Example:

`-0.1 g`

`-0.2 g`

Result:

**HX711 + DHT22 successfully coexisted.**

### HX711 + DHT22 + Ultrasonic

The ultrasonic sensor was then reconnected.

The HX711 remained stable around:

`-0.1 to -0.3 g`

Result:

**HX711 + DHT22 + Ultrasonic successfully coexisted.**

### HX711 + DHT22 + Ultrasonic + RC522

The RC522 was then added.

The HX711 continued to remain close to zero.

This demonstrated that the four hardware subsystems could coexist electrically when correctly connected.

---

## RC522 Troubleshooting

When the RC522 was tested during the combined hardware configuration, the standalone RFID program initially returned:

`RC522 Version Register: 0x00`

followed by:

`ERROR: RC522 not communicating.`

This indicated that the ESP32-S3 was not receiving a valid SPI response from the RC522.

The RC522 connections were checked and rewired.

The successful SPI configuration remained:

| RC522 Pin | ESP32-S3 |
|-----------|-----------|
| SDA / SS | GPIO10 |
| MOSI | GPIO11 |
| SCK | GPIO12 |
| MISO | GPIO13 |
| RST | GPIO14 |
| GND | GND |
| 3.3V | 3.3V |
| IRQ | Not connected |

After correcting the connection, the standalone RC522 test returned:

`RC522 Version Register: 0x92`

followed by:

`SUCCESS: RC522 detected.`

The RFID tag was then successfully detected:

`RFID UID: 23:31:7F:33`

This confirmed that the RC522 module itself was functioning correctly.

The earlier failure was therefore attributed to a wiring/contact problem rather than a software or hardware incompatibility.

---

## Final All-Sensor Test

After correcting the wiring issues, `13_All_Sensors_Test.ino` was uploaded again.

All four subsystems operated successfully in the same ESP32-S3 program.

Example simultaneous sensor output was:

```text
Temperature : 22.8 C
Humidity    : 55.6 %
Distance    : 55.9 cm
Weight      : -0.1 g




## ESP32-S3 All-Sensor MQTT Integration Test

### Objective

The objective of this experiment was to extend the successfully verified multi-sensor ESP32-S3 system by adding Wi-Fi and MQTT communication.

The goal was to verify that all four sensor subsystems could operate simultaneously while the ESP32-S3 also:

- connected to Wi-Fi,
- connected to the MQTT broker,
- published continuous sensor telemetry,
- and published RFID scan events.

The integrated system included:

- DHT22 temperature and humidity sensor
- M5Stack Ultrasonic-I2C distance sensor
- HX711 load cell amplifier and load cell
- RC522 RFID reader
- ESP32-S3
- Wi-Fi
- MQTT communication

---

### System Architecture

The tested architecture was:

DHT22  
+ Ultrasonic-I2C  
+ HX711 Load Cell  
+ RC522 RFID  
→ ESP32-S3  
→ Wi-Fi  
→ MQTT Broker  
→ Sensor Telemetry Topic + RFID Event Topic

---

### Arduino Program

A new Arduino sketch was created:

`14_All_Sensors_MQTT.ino`

The program combined:

- multi-sensor acquisition,
- RFID event detection,
- Wi-Fi communication,
- MQTT communication,
- JSON message generation,
- and periodic telemetry publishing.

---

### Final Pin Configuration

| Device | Function | ESP32-S3 Pin |
|---|---|---|
| HX711 | DAT | GPIO4 |
| HX711 | CLK | GPIO5 |
| DHT22 | DATA | GPIO7 |
| Ultrasonic | SDA | GPIO8 |
| Ultrasonic | SCL | GPIO9 |
| RC522 | SDA / SS | GPIO10 |
| RC522 | MOSI | GPIO11 |
| RC522 | SCK | GPIO12 |
| RC522 | MISO | GPIO13 |
| RC522 | RST | GPIO14 |

### Power Configuration

| Device | Power |
|---|---|
| DHT22 | 3.3V |
| RC522 | 3.3V |
| Ultrasonic-I2C | 5V |
| HX711 VDD | 3.3V |
| HX711 VCC | 5V |
| All devices | Common GND |

The HX711 calibration factor remained:

`-208.45`

---

## MQTT Configuration

The MQTT broker used was:

`broker.hivemq.com`

Two MQTT topics were used.

### Sensor Telemetry Topic

`savonia/mahfuz/iot_thesis/telemetry`

This topic carried:

- temperature,
- humidity,
- distance,
- weight,
- device ID,
- and ESP32 uptime.

### RFID Event Topic

`savonia/mahfuz/iot_thesis/rfid`

This topic carried:

- device ID,
- RFID scan event,
- RFID UID,
- and ESP32 uptime.

---

## Telemetry JSON Structure

The sensor telemetry message used the following structure:

```json
{
  "device_id": "esp32_s3_01",
  "temperature_c": 22.1,
  "humidity_pct": 55.7,
  "distance_cm": 56.3,
  "weight_g": 999.5,
  "uptime_ms": 123456
}



## Raspberry Pi Combined Inventory MQTT Processing Test

### Objective

The objective of this experiment was to integrate both ESP32-S3 MQTT data streams into a single Python application running on the Raspberry Pi 5.

The previous stage had already confirmed that the ESP32-S3 could successfully publish:

1. continuous sensor telemetry, and
2. RFID scan events

through separate MQTT topics.

The purpose of this experiment was to verify that the Raspberry Pi could:

- subscribe to both topics simultaneously,
- parse both JSON message types,
- maintain the latest sensor state,
- identify a scanned RFID tag,
- map the RFID UID to an inventory item,
- and associate the RFID event with the most recent sensor measurements.

---

### System Architecture

The tested data flow was:

DHT22  
+ Ultrasonic-I2C  
+ HX711 Load Cell  
+ RC522 RFID  
→ ESP32-S3  
→ Wi-Fi  
→ MQTT Broker  
→ Raspberry Pi 5  
→ Python MQTT Application  
→ Product Identification + Latest Sensor State

Two MQTT topics were used:

```text
savonia/mahfuz/iot_thesis/telemetry

and

savonia/mahfuz/iot_thesis/rfid




## Raspberry Pi Inventory Stock-In and Stock-Out Logic Test

### Objective

The objective of this experiment was to extend the Raspberry Pi inventory-processing application from passive monitoring into active inventory state management.

The system had already successfully demonstrated:

- RFID product identification,
- continuous sensor telemetry,
- weight measurement,
- MQTT communication,
- and association of RFID events with the latest sensor state.

The next goal was to determine whether the Raspberry Pi could use RFID identification together with load-cell measurements to automatically determine whether an inventory item was being added to or removed from stock.

---

### Python Program

A new Python program was created:

`05_Inventory_State_Logic.py`

The program subscribed to:

`savonia/mahfuz/iot_thesis/telemetry`

and:

`savonia/mahfuz/iot_thesis/rfid`

The program maintained an inventory state for the prototype product:

| Field | Value |
|---|---|
| RFID UID | `23:31:7F:33` |
| Product ID | `Product_001` |
| Name | `Test Inventory Item` |
| Category | `Prototype` |
| Expected weight | `1000 g` |
| Initial state | `OUT` |
| Initial quantity | `0` |

---

## Inventory Decision Logic

The inventory state was determined using both:

- the current RFID event,
- and the latest load-cell weight.

### Stock-In Rule

When the current product state was:

`OUT`

the system expected the physical item to be present on the load cell.

For the prototype test, the expected product weight was:

`1000 g`

with a temporary tolerance of:

`±100 g`

Therefore, the accepted stock-in range was:

`900–1100 g`

If:

- the RFID tag was recognized,
- recent telemetry was available,
- and the measured weight was within this range,

the inventory state changed from:

`OUT → IN`

and quantity changed from:

`0 → 1`

---

### Stock-Out Rule

When the current product state was:

`IN`

the system expected the item to have been removed from the load cell before confirming stock-out.

For the prototype, a zero-weight threshold of:

`±100 g`

was used.

If:

- the RFID tag was recognized,
- recent telemetry was available,
- and the measured weight was close to zero,

the inventory state changed from:

`IN → OUT`

and quantity changed from:

`1 → 0`

---

### Telemetry Freshness Rule

To avoid using outdated sensor measurements, RFID transactions were accepted only when recent telemetry was available.

The maximum accepted telemetry age was configured as:

`6 seconds`

If the latest telemetry was older than this value, the transaction would be rejected.

---

## Stock-In Test

The inventory application initially contained:

```text
Current state : OUT
Current qty   : 0





## SQLite Persistent Inventory Database Test

### Objective

The objective of this experiment was to extend the Raspberry Pi inventory system with persistent storage using SQLite.

The previous inventory application successfully handled:

- RFID product identification,
- stock-in logic,
- stock-out logic,
- quantity updates,
- telemetry association,
- and in-memory inventory state.

However, the inventory state was lost whenever the Python application stopped.

The goal of this experiment was therefore to store inventory state and transaction history permanently so that the system could recover its previous state after application or Raspberry Pi restart.

---

### Database Technology

SQLite was selected as the prototype database.

SQLite was suitable for this stage because:

- it runs locally on the Raspberry Pi,
- it does not require a separate database server,
- it is supported directly by Python through `sqlite3`,
- and it is sufficient for prototype inventory storage and transaction logging.

The database file was created as:

`inventory.db`

Location:

`~/IoT_Thesis/Software/RaspberryPi/inventory.db`

---

## Database Initialization

A new Python script was created:

`06_Database_Init.py`

The script created three database tables:

1. `products`
2. `inventory_state`
3. `inventory_transactions`

Foreign-key enforcement was enabled using:

`PRAGMA foreign_keys = ON`

---

## Products Table

The `products` table stores relatively permanent product information.

The prototype product was inserted as:

| Field | Value |
|---|---|
| Product ID | `Product_001` |
| RFID UID | `23:31:7F:33` |
| Name | `Test Inventory Item` |
| Category | `Prototype` |
| Expected weight | `1000.0 g` |
| Weight tolerance | `100.0 g` |

The RFID UID was defined as unique so that one RFID identifier maps to one known product.

---

## Inventory State Table

The `inventory_state` table stores the latest inventory condition of each product.

The prototype state was initialized as:

```text
Product ID : Product_001
Status     : OUT
Quantity   : 0

## Multi-Product and Multi-Quantity Database Schema V2 Test

### Objective

The objective of this phase was to extend the inventory-management prototype beyond the original single-product, binary quantity model.

The previous database implementation successfully supported:

- one product,
- one RFID-tagged item,
- quantity values of 0 or 1,
- persistent stock-in and stock-out,
- and permanent transaction history.

The next goal was to create a database structure capable of supporting:

- multiple physical items,
- multiple RFID tags,
- several physical units belonging to the same product,
- aggregate product quantities greater than one,
- and item-level inventory status.

---

## Limitation of the Previous Database Model

The original database stored the RFID UID directly inside the `products` table.

Conceptually:

```text
Product_001
    RFID UID: 23:31:7F:33



## Real-Time Multi-Quantity Inventory Integration Test

### Objective

The objective of this experiment was to validate the Schema V2 multi-quantity inventory model in real time using:

- live ESP32-S3 sensor telemetry,
- MQTT communication,
- RFID item identification,
- SQLite persistence,
- item-level inventory state,
- aggregate product quantity,
- and aggregate weight validation.

The test combined one physically scanned RFID item with one simulated second item.

---

### Python Program

A new Raspberry Pi application was created:

`12_RealTime_MultiQuantity.py`

The program combined:

- MQTT telemetry subscription,
- MQTT RFID event subscription,
- SQLite Schema V2,
- item-level IN/OUT state,
- aggregate product quantity,
- live weight validation,
- persistent transaction storage,
- and a simulation command for the second inventory item.

---

## Initial Database State

The test started with:

```text
Item_001 -> OUT
Item_002 -> OUT
Product_001 quantity -> 0




## Demand Forecasting Machine Learning Pipeline Test

### Objective

The objective of this phase was to develop and evaluate a machine-learning pipeline for inventory demand forecasting.

Because the real IoT inventory system had only recently started collecting transaction data, there was not yet enough historical real-world demand data to train a meaningful forecasting model.

Therefore, a clearly labeled synthetic dataset was created exclusively for ML pipeline development and testing.

Synthetic observations were not treated as real experimental results.

---

### Synthetic Demand Dataset

The program:

`16_Synthetic_Demand_Dataset.py`

generated 180 days of synthetic daily demand data for:

`Product_001`

The dataset period was:

`2026-04-03` to `2026-09-29`

Dataset statistics:

```text
Observations    : 180
Average demand  : 13.35 units/day
Minimum demand  : 6 units/day
Maximum demand  : 21 units/day



/home/mahfuz/IoT_Thesis/Dataset/actual_vs_predicted.png ~/Projects/IoT_Thesis/Dataset/

/home/mahfuz/IoT_Thesis/Dataset/model_mae_comparison.png ~/Projects/IoT_Thesis/Dataset/




## Forecast-Driven Inventory Optimization Test

### Objective

The objective of this experiment was to connect the demand-forecasting output with an inventory replenishment decision.

The inventory optimization prototype combined:

- predicted future demand,
- current inventory quantity from SQLite,
- lead-time demand,
- safety stock,
- reorder point,
- target stock,
- and recommended order quantity.

---

### Forecast Input

The Linear Regression forecasting pipeline generated a seven-day synthetic demand forecast:

| Date | Predicted Demand |
|---|---:|
| 2026-09-30 | 13 |
| 2026-10-01 | 14 |
| 2026-10-02 | 15 |
| 2026-10-03 | 19 |
| 2026-10-04 | 19 |
| 2026-10-05 | 15 |
| 2026-10-06 | 16 |

Total predicted seven-day demand:

`111 units`

Average predicted daily demand:

`15.86 units/day`

The forecast was based on synthetic demand history and was used only to validate the optimization pipeline.

---

### Prototype Inventory Policy

The following prototype assumptions were used:

```text
Lead time           : 3 days
Safety-stock cover  : 2 days



## 2026-10-01 – Future Demand Forecasting

### Objective

The objective of this experiment was to extend the machine-learning pipeline from historical model evaluation to future demand prediction.

The program:

`22_Future_Demand_Forecast.py`

was created on the Raspberry Pi.

The model used for the forecasting demonstration was:

`Linear Regression`

Linear Regression was selected because it had produced the lowest mean MAE and RMSE during the five-fold time-series cross-validation performed on the synthetic demand dataset.

### Training Data

The final model used:

`166 ML-usable observations`

derived from the synthetic demand dataset.

The historical dataset ended on:

`2026-09-29`

The forecasting horizon was:

`7 days`

### Forecast Results

The following predictions were generated:

| Date | Predicted Demand |
|---|---:|
| 2026-09-30 | 13 units |
| 2026-10-01 | 14 units |
| 2026-10-02 | 15 units |
| 2026-10-03 | 19 units |
| 2026-10-04 | 19 units |
| 2026-10-05 | 15 units |
| 2026-10-06 | 16 units |

Total predicted demand:

`111 units`

Average predicted daily demand:

`15.86 units/day`

The forecast was saved to:

`future_demand_forecast.csv`

A visualization was also generated:

`future_demand_forecast.png`

Each forecast record was explicitly labeled:

`SYNTHETIC_FORECAST`

### Important Limitation

The forecast was generated from synthetic historical demand data.

Therefore, the values demonstrate the forecasting pipeline but do not represent validated real-world demand predictions.

### Status

**Future demand forecasting pipeline: SUCCESSFUL**

---

## 2026-10-01 – Forecast-Driven Reorder Recommendation

### Objective

The objective of this experiment was to connect the demand-forecasting output with the inventory-management system.

The program:

`23_Reorder_Recommendation.py`

combined:

- future demand forecast,
- current inventory quantity from SQLite,
- lead-time demand,
- safety stock,
- reorder point,
- target stock,
- and recommended purchase quantity.

### Prototype Parameters

The following prototype values were used:

```text
Lead time           : 3 days
Safety-stock cover  : 2 days




## 2026-10-11 – Finland Market-Informed ML and Inventory Optimization Final Evaluation

### Objective

The final experimental phase extended the original proof-of-concept machine-learning pipeline into a more realistic controlled Finland-oriented simulation environment.

The objective was to evaluate:

- forecasting performance under longer-term seasonal demand,
- Finnish calendar and retail-related demand effects,
- recursive multi-day forecasting,
- forecast-driven replenishment,
- and inventory-policy trade-offs.

The product-level demand remained semi-synthetic and was not treated as observed Finnish commercial sales data.

---

### Finland Market-Informed Demand Environment

The program:

`26_Finland_Market_Simulation.py`

generated a controlled daily demand dataset covering:

`2023-01-01` to `2026-09-30`

Total observations:

`1,369 days`

Data classification:

`SEMI_SYNTHETIC_FINLAND`

The simulated environment included:

- weekday demand variation,
- monthly seasonality,
- Finnish holidays and special days,
- Christmas-shopping period,
- Black Friday period,
- planned promotions,
- market-context variation,
- random consumer variation,
- and unexpected positive and negative demand shocks.

The random shock variables and generator-specific multipliers were stored separately and were not exposed directly to the forecasting models.

---

### Advanced Feature Engineering

The program:

`27_Finland_Market_Feature_Engineering.py`

generated an expanded ML feature set.

The feature categories included:

- calendar variables,
- cyclical calendar encoding,
- Finnish market/event context,
- demand lags,
- rolling averages,
- rolling demand variability,
- and long-term time position.

Historical demand features included:

```text
lag_1
lag_7
lag_14
lag_28

rolling_7_mean
rolling_14_mean
rolling_28_mean

rolling_7_std
rolling_14_std
rolling_28_std


