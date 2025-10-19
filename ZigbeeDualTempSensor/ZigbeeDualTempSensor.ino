// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief This example demonstrates Zigbee temperature sensor.
 *
 * The example demonstrates how to use Zigbee library to create a end device temperature sensor.
 * The temperature sensor is a Zigbee end device, which is controlled by a Zigbee coordinator.
 *
 * Proper Zigbee mode must be selected in Tools->Zigbee mode
 * and also the correct partition scheme must be selected in Tools->Partition Scheme.
 *
 * Please check the README.md for instructions and more detailed description.
 *
 * Created by Jan Procházka (https://github.com/P-R-O-C-H-Y/)
 */

#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 16;  //D6, gpio16
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"
#define TEMP_MEASURE_INTERVAL_MS 1000
#define ZIGBEE_REPORTING_INTERVAL_SEC 10

/* Zigbee temperature sensor configuration */
#define TEMP_SENSOR_ENDPOINT_NUMBER 10
uint8_t button = BOOT_PIN;

// Optional Time cluster variables
struct tm timeinfo;
struct tm *localTime;
int32_t timezone;

ZigbeeTempSensor zbTempSensor1 = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);
ZigbeeTempSensor zbTempSensor2 = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER + 1);

/************************ Temp sensor *****************************/
static void temp_sensor_value_update(void *arg) {
  for (;;) {
    // Read temperature sensor value
    sensors.requestTemperatures();
    delay(750);

    float temp0 = sensors.getTempCByIndex(0);
    Serial.printf("Updated temperature sensor0 value to %.2f°C\r\n", temp0);
    zbTempSensor1.setTemperature(temp0);

    float temp1 = sensors.getTempCByIndex(1);
    Serial.printf("Updated temperature sensor1 value to %.2f°C\r\n", temp1);
    zbTempSensor2.setTemperature(temp1);

    delay(TEMP_MEASURE_INTERVAL_MS);
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  pinMode(oneWireBus, INPUT);
  sensors.begin();

  // Init button switch
  pinMode(button, INPUT_PULLUP);

  // Optional: set Zigbee device name and model
  zbTempSensor1.setManufacturerAndModel("Espressif", "ZigbeeTempSensor");

  // Set minimum and maximum temperature measurement value (10-50°C is default range for chip temperature measurement)
  zbTempSensor1.setMinMaxValue(10, 60);
  zbTempSensor2.setMinMaxValue(10, 60);

  // Optional: Set tolerance for temperature measurement in °C (lowest possible value is 0.01°C)
  zbTempSensor1.setTolerance(0.1);
  zbTempSensor2.setTolerance(0.1);

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbTempSensor1);
  Zigbee.addEndpoint(&zbTempSensor2);

  Serial.println("Starting Zigbee...");
  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Start Temperature sensor reading task
  xTaskCreate(temp_sensor_value_update, "temp_sensor_update", 2048, NULL, 10, NULL);

  // Set reporting interval for temperature measurement in seconds, must be called after Zigbee.begin()
  // min_interval and max_interval in seconds, delta (temp change in 0,1 °C)
  // if min = 1 and max = 0, reporting is sent only when temperature changes by delta
  // if min = 0 and max = 10, reporting is sent every 10 seconds or temperature changes by delta
  // if min = 0, max = 10 and delta = 0, reporting is sent every 10 seconds regardless of temperature change
  zbTempSensor1.setReporting(0, ZIGBEE_REPORTING_INTERVAL_SEC, 0.1);
  zbTempSensor2.setReporting(0, ZIGBEE_REPORTING_INTERVAL_SEC, 0.1);
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbTempSensor1.reportTemperature();
    zbTempSensor2.reportTemperature();
  }
  delay(100);
}
