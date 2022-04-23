#include <Arduino.h>
#include <esp32_can.h>
#include "wifi/wifi.h"
#include "mqtt/mqtt.h"
#include "ota/ota.h"
#include "secrets.h"
CAN_FRAME canMessage;
CAN_FRAME txCanMessage;
comfoair::WiFi *wifi;
comfoair::MQTT *mqtt;
comfoair::OTA *ota;
char buf[250];
uint8_t buf_b[64];
char topicBuf[32];

void brocca (char const * topic, uint8_t const * payload, int payload_length) {
  Serial.println(topic);

  txCanMessage.id = atoi(strrchr(topic, '/') + 1);
  Serial.println(txCanMessage.id);
  memcpy((void * ) payload, &txCanMessage.timestamp, 4);
  uint8_t flags = payload[9];
  txCanMessage.extended = 0;
  txCanMessage.rtr = flags & 0x2;
  txCanMessage.length = payload_length - 9;
  for (int i=0; i< txCanMessage.length; i++) {
    txCanMessage.data.byte[i] = payload[9+i];
  }

  CAN0.sendFrame(txCanMessage);

}
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
  mqtt->subscribeTo("can2/s/#", brocca);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (CAN0.read(canMessage)) {
      // SavvyCan support
      uint8_t flags = 0;
      if (canMessage.extended) flags +=1;
      if (canMessage.rtr) flags +=2;
      
      sprintf(topicBuf, "can2/%d", canMessage.id);
      // if (canMessage.is_fd) flags +=4;
      // if (canMessage.error_state_indicator) flags +=8;
      memcpy(buf_b, &canMessage.timestamp, 4);
      memcpy(buf_b+8, &flags, 1);
      for (int i = 0; i < canMessage.length; i++) {
        buf_b[9+i] = canMessage.data.byte[i];
      }
      mqtt->writeToTopic(topicBuf, buf_b, 9+canMessage.length);
      Serial.println(buf);
  }
  ota->loop();
  wifi->loop();
  mqtt->loop();
}