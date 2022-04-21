#include <PubSubClient.h>
#include <WiFi.h>
#include <map>
#include "../secrets.h"
#include "mqtt.h"

namespace comfoair {

WiFiClient wifiClient;
  MQTT::MQTT() {
    this->client = PubSubClient(wifiClient);
  }

  void MQTT::subscribeTo(char* const topic, MQTT_CALLBACK_SIGNATURE) {
    this->callbackMap[topic] = callback;
    if (this->client.connected()) {
      this->subscribeToTopics();
    }
  }

  void MQTT::setup() {
    this->client.setServer(MQTT_HOST, MQTT_PORT);
  }

  void MQTT::loop() {
    this->ensureConnected();
    client.loop();
  }

  void MQTT::writeToTopic(char * topic, char * payload) {
    this->ensureConnected();
    this->client.publish(topic, payload);
  }

// PRIVATE STUFF

  void MQTT::ensureConnected() {
    // Loop until we're reconnected
    while (!this->client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        // Attempt to connect
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
            Serial.println("connected");
            subscribeToTopics();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
  }

  void MQTT::subscribeToTopics() {
    std::map<std::string, std::function<void(char*, uint8_t*, unsigned int)>>::iterator it;
    for (it=callbackMap.begin(); it!=callbackMap.end(); ++it) {
      std::string s = it->first;
      Serial.print("Subscribing to: ");
      Serial.println(s.c_str());
      client.subscribe(s.c_str());
    }
  }

} // namespace comfoair