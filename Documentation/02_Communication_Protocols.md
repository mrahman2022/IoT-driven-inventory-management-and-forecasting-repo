# Communication Protocols

## Why do sensors need communication?

The ESP32 cannot understand a sensor directly.

A communication protocol defines how data is exchanged between the ESP32 and external devices.

---

## Communication methods used in this thesis

| Protocol | Used By |
|-----------|----------|
| Digital | LED, HX711 |
| Analog | MaxBotix MB1010 |
| I²C | SHT31, PN532 |
| SPI | MicroSD Module |

---

## Digital

Simple ON/OFF communication.

Example:
- LED
- Push Button

---

## Analog

Voltage represents data.

Example:
- Ultrasonic sensor
- Potentiometer

---

## I²C

Uses only two communication wires.

SDA = Data

SCL = Clock

Allows multiple devices on the same bus.

---

## SPI

High-speed communication.

Uses:

- MOSI
- MISO
- SCK
- CS

Used for MicroSD storage.