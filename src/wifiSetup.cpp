#include "wifiSetup.hpp"
#include <Arduino.h>
#include "tempo.hpp"
#include <string.h>

wifiSetup::wifiSetup() {}

void wifiSetup::connect(const char *ssid, const char *password){
  _ssid = ssid;
  _password = password;
#if defined(DEBUG_ALL) || defined(DEBUG_COAP) || defined(DEBUG_MQTT)
  Serial.print("Trying to connect to ");
  Serial.print(ssid);
#endif
  WiFi.mode(WIFI_STA); // indica para agir apenas como wifi_client, o default eh agir tanto como wifi_client e um access-point.
  //tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA ,"ESP32-HEHEHEHE");
  // tenta conectar a rede wifi
  WiFi.begin(ssid, password);
  // aguarda a conexao ocorrer
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
#if defined(DEBUG_ALL) || defined(DEBUG_COAP) || defined(DEBUG_MQTT)
      Serial.print(".");
#endif
  }

#if defined(DEBUG_ALL) || defined(DEBUG_COAP) || defined(DEBUG_MQTT)
  // informa via serial o endereco IP obtido
  Serial.println("");
  //initSerialStamp();
  Serial.println("WiFi conectado");
  //initSerialStamp();
  Serial.println("Endereco IP: ");
  //initSerialStamp();
  Serial.println(WiFi.localIP());
#endif
}

void wifiSetup::reconnect() {
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    WiFi.begin(_ssid, _password); // se nao estiver, tenta conectar
#if defined(DEBUG_ALL) || defined(DEBUG_COAP) || defined(DEBUG_MQTT)
    printSerialTimeStamp();
    Serial.print("WiFi desconectado, tentando reconectar!!");
#endif
  }
}

void wifiSetup::checkConnection() {
  if(WiFi.status() == WL_CONNECTED){
    reconnect();
  }
}

void wifiSetup::macAddress(char buffer[18]) {
  strcpy(buffer, WiFi.macAddress().c_str());
}

IPAddress wifiSetup::localIP(){
  return WiFi.localIP();
}
