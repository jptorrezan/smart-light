#include "mqttSetup.hpp"
#include "tempo.hpp"
#include <Arduino.h>

MqttSetup::MqttSetup(const uint8_t *mqtt_broker_ip, uint16_t port, MQTT_CALLBACK_SIGNATURE) {
  randomSeed(esp_random());
  MQTT_client.setServer(mqtt_broker_ip, port);
  MQTT_client.setCallback(callback);
}

MqttSetup::MqttSetup(IPAddress mqtt_broker_ip, uint16_t port, MQTT_CALLBACK_SIGNATURE){
  randomSeed(esp_random());
  MQTT_client.setServer(mqtt_broker_ip, port);
  MQTT_client.setCallback(callback);
}

MqttSetup::MqttSetup(const char *mqtt_broker_domain, uint16_t port, MQTT_CALLBACK_SIGNATURE){
  randomSeed(esp_random());
  MQTT_client.setServer(mqtt_broker_domain, port);
  MQTT_client.setCallback(callback);
}

/*MqttSetup::~MqttSetup() {
  delete &_subs;
}*/

void MqttSetup::reconnect() {
  int i;
  char macadd[18];
  // Loop until we're reconnected
  while (!MQTT_client.connected()) {
    if (_wifi){
      _wifi->checkConnection();
      _wifi->macAddress(macadd);
    }
    printSerialTimeStamp();
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID if does not have a wifiSetup
    String clientId = "ESP32Client-";
    clientId += (_wifi ? String(macadd) : "rand:" + String(random(0xffff), HEX));
    Serial.println(clientId.c_str());
    // Attempt to connect
    if (MQTT_client.connect(clientId.c_str())) {
      Serial.println(" connected");
      // Once connected, publish an announcement...
      //Serial.println("publishing temp");
      //MQTT_client.publish("temp", "0");
      // ... and resubscribe
      for(i = 0; i < _subs.size(); i++) {
        printSerialTimeStamp();
        Serial.print("subscribing ");
        Serial.println((const char*)_subs[i].c_str());
        if(MQTT_client.subscribe((const char*)_subs[i].c_str()), 1){
          printSerialTimeStamp();
          Serial.println("Subscribed successfully");
        }
      }
    } else {
      printSerialTimeStamp();
      Serial.print("failed, rc=");
      Serial.print(MQTT_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MqttSetup::checkConnection(){
  if(!MQTT_client.connected()){
    reconnect();
  }
}

void MqttSetup::checkMsgs(){
  this->checkConnection();
  MQTT_client.loop();
}

void MqttSetup::addSubscription(const char *topic, const char *topic_modifier) {
  if (topic_modifier[0] == 0) {
    _subs.emplace_back(topic);
  }
  else {
    char temp_buffer[MQTT_TOPIC_SIZE];
    String tempTopic(topic);
    String tempTopic_modifier(topic_modifier);
    if ((tempTopic.end() - 1)[0] != '/') {
      if (tempTopic_modifier[0] != '/') {
        _subs.emplace_back(tempTopic + "/" + tempTopic_modifier);
      }
      else {
        _subs.emplace_back(tempTopic + tempTopic_modifier);
      }
    }
    else {
      if ((tempTopic_modifier.end() - 1)[0] != '/') {
        _subs.emplace_back(tempTopic + tempTopic_modifier);
      }
      else {
        _subs.emplace_back(tempTopic + tempTopic_modifier.substring(1));
      }
    }
  }
}

void MqttSetup::publish(const char *topic, const char *payload, bool retained) {
  this->checkConnection();
  MQTT_client.publish(topic, payload, retained);
}

void MqttSetup::setWifi(wifiSetup* wifi){
  _wifi = wifi;
}

void MqttSetup::printBufferSize(){
  Serial.print("Buffer size:");
  Serial.println(this->MQTT_client.getBufferSize());
  }
