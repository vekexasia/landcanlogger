#include <Arduino.h>
#include <esp32_can.h>
#include "wifi/wifi.h"
#include "mqtt/mqtt.h"
#include "ota/ota.h"
#include "secrets.h"
CAN_FRAME canMessage;
comfoair::WiFi *wifi;
comfoair::MQTT *mqtt;
comfoair::OTA *ota;
char buf[250];

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  CAN0.setCANPins(GPIO_NUM_17, GPIO_NUM_16);
  CAN0.setDebuggingMode(false);
  CAN0.begin(500000);
  CAN0.watchFor();
  
  wifi = new comfoair::WiFi();
  mqtt = new comfoair::MQTT();
  ota = new comfoair::OTA();
  wifi->setup();
  mqtt->setup();
  ota->setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (CAN0.read(canMessage)) {
      sprintf(buf, "%08X,", canMessage.id);
      for (int i = 0; i < canMessage.length; i++) {
        sprintf(&buf[9+(i*3)], "%02X ", canMessage.data.byte[i]);
      }

      mqtt->writeToTopic(MQTT_PREFIX, buf);

      // SavvyCan support
      uint8_t flags = 0;
      if (canMessage.extended) flags +=1;
      if (canMessage.rtr) flags +=2;
      // if (canMessage.is_fd) flags +=4;
      // if (canMessage.error_state_indicator) flags +=8;
      canMessage.timestamp
      Serial.println(buf);
  }
  ota->loop();
  wifi->loop();
  mqtt->loop();
}