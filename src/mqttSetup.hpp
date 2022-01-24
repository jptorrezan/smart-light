#ifndef MQTT_SETUP_H
#define MQTT_SETUP_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "wifiSetup.hpp"
#include <WiFiClient.h>

// Header + Topico + Payload + 2 deve ser menor que MQTT_MAX_PACKET_SIZE definido na PubSubClient.h
#define MQTT_MSG_SIZE MQTT_MAX_PACKET_SIZE-MQTT_MAX_HEADER_SIZE-2+1
#define MQTT_TOPIC_SIZE 50


class MqttSetup{
  private:
    WiFiClient _espClient;
    //unsigned int _pubs_count;
    //unsigned int _subs_count;
    std::vector<String> _subs;
    wifiSetup* _wifi { nullptr };
  public:
    MqttSetup(const uint8_t *mqtt_broker_ip, uint16_t port, MQTT_CALLBACK_SIGNATURE);
    MqttSetup(IPAddress mqtt_broker_ip, uint16_t port, MQTT_CALLBACK_SIGNATURE);
    MqttSetup(const char *mqtt_broker_domain, uint16_t port, MQTT_CALLBACK_SIGNATURE);
    //~ MqttSetup();
    PubSubClient MQTT_client { PubSubClient(_espClient) };
    char MQTT_msg[MQTT_MSG_SIZE];
    void checkConnection();
    void checkMsgs();
    void reconnect();
    void addSubscription(const char *topic, const char *topic_modifier = "");
    void publish(const char *topic, const char *payload, bool retained = false);
    void setWifi(wifiSetup* wifi);
    void printBufferSize();
};
#endif
