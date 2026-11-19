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

  // Other sensor or communication tasks can run here.
}