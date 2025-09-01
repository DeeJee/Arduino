/*
The sketch is tested on Xiao-C6 and Zigbee2MQTT.
Arduino-ESP32 core 3.1.0 was used.
The sensor is powered from D3 pin to achieve low power consumption.
Average consumption during deep sleep is 14uA @ 4.2V.
Code execution takes around 2.6s and consumes on average around 60mA @ 4.2V.
*/



#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif


#include "Zigbee.h"
#include "esp_sleep.h"

#include <BH1750.h>
BH1750 lightMeter;

#define ZIGBEE_ILLUMINANCE_SENSOR_ENDPOINT 9
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 55          /* Sleep for 55s will + 5s delay for establishing connection => data reported every 1 minute */
#define DELAY 100

uint8_t button = BOOT_PIN;
uint8_t sensePower = D3;


ZigbeeIlluminanceSensor zbIlluminanceSensor = ZigbeeIlluminanceSensor(ZIGBEE_ILLUMINANCE_SENSOR_ENDPOINT);

float getBatteryVoltage() {
  // Optional: configure analog input
  analogSetAttenuation(ADC_11db);  // set analog to digital converter (ADC) attenuation to 11 dB (up to ~3.3V input)
  analogReadResolution(12);        // set analog read resolution to 12 bits (value range from 0 to 4095), 12 is default
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) {
    Vbatt = Vbatt + analogReadMilliVolts(A0);  // ADC with correction
  }
  return 2 * Vbatt / 16 / 1000.0;  // attenuation ratio 1/2, mV --> V
}



static uint16_t temp_sensor_value_update(void) {
  float voltage = getBatteryVoltage();
  Serial.printf("voltage %f", voltage);
  Serial.println();

  float roundedValue = round(voltage * 10) / 10;
  Serial.printf("roundedValue %f", roundedValue);
  Serial.println();

  int voltageTimesTen = (int)(10 * (roundedValue));
  Serial.printf("voltageTimesTen %d", voltageTimesTen);
  Serial.println();

  if (!zbIlluminanceSensor.setBatteryPercentage(voltageTimesTen)) {
    Serial.println("setBatteryPercentage failed!");
  }

  uint16_t lux = lightMeter.readLightLevel();

  // read the raw analog value from the sensor
  Serial.printf("[Illuminance Sensor] raw analog value: %d\r\n", lux);

  // Update illuminance in illuminance sensor EP
  uint16_t encodedVaule = 10000 * log10(lux + 1);
  Serial.printf("[Illuminance Sensor] encoded value: %d\r\n", encodedVaule);
  if (!zbIlluminanceSensor.setIlluminance(encodedVaule)) {
    Serial.println("Updating illuminance failed!");
  }

  zbIlluminanceSensor.report();
  zbIlluminanceSensor.reportBatteryPercentage();
  Serial.flush();
  delay(100);
  return lux;
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  //  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Init button switch
  pinMode(button, INPUT_PULLUP);
  pinMode(A0, INPUT);  // ADC
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Wire.begin();
  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
  // Set Zigbee device name and model
  zbIlluminanceSensor.setManufacturerAndModel("Espressif", "ZigbeeIlluminanceSensor");

  // Set minimum and maximum for raw illuminance value (0 min and 50000 max equals to 0 lux - 100,000 lux)
  if (!zbIlluminanceSensor.setMinMaxValue(0, 65535)) {
    Serial.println("setMinMaxValue failed");
  }

  // Optional: Set tolerance for raw illuminance value
  if (!zbIlluminanceSensor.setTolerance(1)) {
    Serial.println("setTolerance failed");
  }

  zbIlluminanceSensor.setPowerSource(ZB_POWER_SOURCE_BATTERY, 100);

  // Add endpoint to Zigbee Core
  Serial.println("Adding Zigbee illuminance sensor endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbIlluminanceSensor);

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

  int zigbeeConnectTime = 0;
  while (!Zigbee.connected() && zigbeeConnectTime < 20000) {
    zigbeeConnectTime += DELAY;
    Serial.print(".");
    delay(DELAY);
  }

  Serial.println();
  // Wait 20s if boot reason is not wake from sleep. This is required for proper pairing with Zigbee2MQTT
  if ((int)esp_rom_get_reset_reason(0) != 5) {  //  SW_CPU_RESET=12 ,  POWERON_RESET=1 , DEEPSLEEP_RESET=5
    Serial.print("Pairing");
    int pairingTime = 0;
    while (pairingTime < 20000) {
      Serial.print(".");
      pairingTime += DELAY;
      delay(DELAY);
    }
    Serial.println();
  }
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {
    delay(DELAY);
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
  }
  uint16_t lux = temp_sensor_value_update();
  Serial.println("Going to sleep now");

  // Put ESP to sleep
  int timeToSleep = lux > 10 ? 55 : 3600;
  Serial.printf("Going to sleep for %d seconds", timeToSleep);
  esp_sleep_enable_timer_wakeup(timeToSleep * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}