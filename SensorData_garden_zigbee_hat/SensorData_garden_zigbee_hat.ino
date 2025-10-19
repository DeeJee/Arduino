/*
The sketch is tested on Xiao-C6 and Zigbee2MQTT.
Arduino-ESP32 core 3.1.0 was used.
The sensor is powered from D3 pin to achieve low power consumption.
Average consumption during deep sleep is 14uA @ 4.2V.
Code execution takes around 2.6s and consumes on average around 60mA @ 4.2V.
*/

#define DEBUG_TRACE

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
#define ENABLEVOLTAGEDIVIDER D10
#define VOLTAGEPIN A1
uint8_t button = BOOT_PIN;
uint8_t sensePower = D3;


ZigbeeIlluminanceSensor zbIlluminanceSensor = ZigbeeIlluminanceSensor(ZIGBEE_ILLUMINANCE_SENSOR_ENDPOINT);

float getBatteryVoltage() {

  // Optional: configure analog input
  analogSetAttenuation(ADC_11db);  // set analog to digital converter (ADC) attenuation to 11 dB (up to ~3.3V input)
  analogReadResolution(12);        // set analog read resolution to 12 bits (value range from 0 to 4095), 12 is default
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) {
    Vbatt = Vbatt + analogReadMilliVolts(VOLTAGEPIN);  // ADC with correction
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
  Serial.printf("[Illuminance Sensor] raw value: %d\r\n", lux);

  // Update illuminance in illuminance sensor EP
  uint16_t encodedValue = 10000 * log10(lux + 1);
  Serial.printf("[Illuminance Sensor] encoded value: %d\r\n", encodedValue);
  if (!zbIlluminanceSensor.setIlluminance(encodedValue)) {
    Serial.println("Updating illuminance failed!");
  }

  zbIlluminanceSensor.report();
  zbIlluminanceSensor.reportBatteryPercentage();

  return lux;
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  //  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Init button switch
  pinMode(button, INPUT_PULLUP);
  pinMode(VOLTAGEPIN, INPUT);  // ADC

  pinMode(ENABLEVOLTAGEDIVIDER, OUTPUT);  // voltage divider
  digitalWrite(ENABLEVOLTAGEDIVIDER, HIGH);
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

  if (Zigbee.connected()) {
    Serial.printf("Connected to zigbee in %d seconds", zigbeeConnectTime / 1000);
    Serial.println();
  } else {
    Serial.println("Timeout connecting to zigbee!!");
  }

  // Wait 20s if boot reason is not wake from sleep. This is required for proper pairing with Zigbee2MQTT
  if ((int)esp_rom_get_reset_reason(0) != 5) {  //  SW_CPU_RESET=12 ,  POWERON_RESET=1 , DEEPSLEEP_RESET=5
    Serial.print("Waiting 10sec for pairing");
    int pairingTime = 0;

    while (pairingTime < 10000) {
      Serial.print(".");
      pairingTime += DELAY;
      delay(DELAY);
    }
    
    Serial.println();
  }
}


void   hybernate(uint16_t lux) {
  // Put ESP to sleep
  int timeToSleep = lux > 10 ? 55 : 3600;
  Serial.printf("Going to sleep for %d seconds", timeToSleep);

  digitalWrite(ENABLEVOLTAGEDIVIDER, LOW);

  Serial.flush();
  delay(100);

  esp_sleep_enable_timer_wakeup(timeToSleep * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  uint16_t lux = temp_sensor_value_update();
  hybernate(lux);
}