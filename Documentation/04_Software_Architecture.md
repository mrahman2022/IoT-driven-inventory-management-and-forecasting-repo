## Raspberry Pi Combined Inventory Processing

The Raspberry Pi 5 now runs a Python MQTT application that subscribes simultaneously to:

`savonia/mahfuz/iot_thesis/telemetry`

and:

`savonia/mahfuz/iot_thesis/rfid`

The application maintains the most recent sensor telemetry in memory and combines it with RFID events.

RFID UID:

`23:31:7F:33`

is currently mapped to:

`Product_001`

During the final integrated test, the RFID event was successfully associated with:

- Temperature: `22.3 °C`
- Humidity: `54.2 %`
- Distance: `55.9 cm`
- Weight: `999.7 g`
- Telemetry age: `0.7 seconds`

This confirms successful end-to-end communication from the physical sensing layer through the ESP32-S3 and MQTT broker to Raspberry Pi application-level processing.

**Status: Combined Raspberry Pi inventory-processing pipeline successfully completed.**





## Inventory State Logic

The Raspberry Pi application has been extended from passive MQTT monitoring to inventory-state processing.

RFID identification and the latest load-cell measurement are evaluated together before inventory state is changed.

The current prototype implements two principal transitions:

`OUT → IN`

when a recognized RFID tag is scanned while the expected item weight is present.

`IN → OUT`

when the recognized item has been removed and the load-cell measurement has returned close to zero.

The prototype also checks telemetry freshness and rejects transactions when sensor information is unavailable, stale, or inconsistent with the expected inventory action.

The current inventory state is stored only in application memory.

Persistent storage is the next software layer and will be implemented using SQLite.




## Real-Time Multi-Quantity Inventory Engine

The Raspberry Pi now runs a real-time inventory engine that combines MQTT telemetry, RFID identification, SQLite Schema V2, and aggregate weight validation.

Each physical inventory unit maintains its own `IN` or `OUT` state, while `inventory_state` stores the total quantity for the corresponding product.

The verified prototype successfully completed the quantity sequence:

`0 -> 1 -> 2 -> 1 -> 0`

using one real RFID-tagged item and one simulated second item.

The engine validates each transaction using the latest load-cell measurement before updating SQLite.

This confirms that the software architecture now supports multi-item and multi-quantity inventory processing in real time.



## Final Software Architecture

The final prototype consists of several connected software layers running across the ESP32-S3, Raspberry Pi, SQLite database, and machine-learning pipeline.

The complete architecture can be represented as:

Physical Sensors and RFID
        ↓
ESP32-S3 Firmware
        ↓
Wi-Fi / MQTT
        ↓
Raspberry Pi MQTT Processing
        ↓
Inventory Logic
        ↓
SQLite Database
        ↓
Transaction and Telemetry Datasets
        ↓
Demand Processing
        ↓
Machine-Learning Pipeline
        ↓
Demand Forecast
        ↓
Inventory Policy Logic
        ↓
Reorder Recommendation

