/***************************************************************************
  This is a library for the BME680 gas, humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME680 Breakout
  ----> http://www.adafruit.com/products/3660

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X); //
  bme.setHumidityOversampling(BME680_OS_2X); //
  bme.setPressureOversampling(BME680_OS_4X); //
}

void loop() {
  if (! bme.performReading()) {
//    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature:"); Serial.print(bme.temperature);      Serial.print(","); // *C
  Serial.print("Pressure:");    Serial.print(bme.pressure / 100.0); Serial.print(","); // hPa
  Serial.print("Humidity:");    Serial.print(bme.humidity);         Serial.print(","); // %
  Serial.println();
  delay(100);
}
