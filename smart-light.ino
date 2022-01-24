//#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <NTPClient.h>
#include <WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <HTTPClient.h>

#include <ArduinoJson.h>
#include "src/tempo.hpp"
//#include <wifiSetup8266.hpp>
#include "src/mqttSetup.hpp"
#include "src/httpUpdateHandler.hpp"


#include "src/lampControl.hpp"

//Arquivo com os endereços e pinos utilizados no projeto
#include "credencials.h"

//Arquivo com a estrutura de tópicos
#include "topics.hpp"




#define USE_SERIAL Serial
#define DEBUG_ALL


//================================== Wifi Controller ========================================

//Criação do objeto responsavel pelo wifi
  //wifiSetup wifiClient;
   WiFiManager wifiManager;

//====================================================================================END WIFI

//================================== Light controller =======================================
 
   
  const bool incial_light_state = true; //OFF
  
  //Criação do Objeto para controle da lampada
  Light_Control lamp(RELAY_PIN, incial_light_state, lightID); 

//===========================================================================END LIGHT CONTROL

//================================= Light Sensor controller =======================================

 
 
const String& firmware_version = "20200620001"; // versao atual: ano-mes-dia-versao do dia

//================================== MQTT Controller ========================================


void sendLightState(MqttSetup mqttClient, char* topic);

void mqtt_callback(char* topic, byte* payload, unsigned int length){
      char payloadString[length + 1];
      for(int i = 0; i < length; i++){
        payloadString[i] = payload[i];      //converte o payload de um vetor de bytes, para string
      }
      payloadString[length] = '\0';
      #ifdef DEBUG_ALL
        printSerialTimeStamp();
        USE_SERIAL.print("Mensagem: ");
        USE_SERIAL.println(payloadString);
        USE_SERIAL.print("Recebida no topico: '");      //print mensagem e tópico
        USE_SERIAL.print(topic);
        USE_SERIAL.println("'");
      #endif
      if(!strcmp(topic, httpsUpload.c_str())){
          //handleUpdate(host, httpsPort, moduleURL, payloadString, firmware_version, fingerprint);
        }
      /*else if(!strcmp(topic, reset_topic.c_str())){
          Serial.println("ESP sera reiniciada");
          ESP.reset();
        }*/
      //Topico de funcionamento das lampadas // ON/OFF
      else if(!strcmp(topic, set_func_topic.c_str()) || !strcmp(topic, set_func_room_topic.c_str()) ){  
        handlePayload(payloadString);
      }
      
      else if(!strcmp(topic, hour_on_topic.c_str())){
          lamp.update_time_on(payload);
        }
      
      
        
    }

//parameters defined in credencials.h
MqttSetup mqttClient(mqtt_broker_ip, mqtt_port, mqtt_callback);
  
//====================================================================================END MQTT

//================================================================================= UTILS MQTT

#define JSON_LIGHT_FIELDS 4
#define MASC 1 

void handlePayload(char* payloadString){
  USE_SERIAL.println("\nFuncao handlePlayload");
  StaticJsonDocument<MAX_TAM_JSON> doc;   
  DeserializationError error = deserializeJson(doc, payloadString); //payload transformado em JSON
  
  if (error) {
    USE_SERIAL.print(F("deserializeJson() failed: "));
    USE_SERIAL.println(error.c_str());                  //flag de erro na deserialização
    return;
  }
  int config = doc["0"];
  USE_SERIAL.print("Byte lido do campo 'config': ");
  USE_SERIAL.println(config);
  int i = 0;
  byte estado; 
  byte politicaVazio; 
  char* horarioOn;
  char* horarioOff;
  const char* msg_id = doc["23"];
  lamp.set_msg_id(msg_id);
  while(i < JSON_LIGHT_FIELDS){
    if(config & MASC ){
        switch(i){
            case 0:
                estado = doc["21"];  
            break;

            case 1:
                politicaVazio = doc["22"];
            break;

            case 2:
                //horarioOn = doc["13"];
            break;

            case 3:
                //horarioOff = doc["14"];
            break;   
        }
    }
    config = config >> 1;
    i++;
  }
  if(doc["12"] == 0){
    switch (estado)
    {
    case 0:
      USE_SERIAL.println("preDesligar Lamp");
      delay(20);
      lamp.set_light_state(true);
      delay(20);
      USE_SERIAL.println("posDesligar Lamp");
      break;
    case 1:
      lamp.set_light_state(false);
      USE_SERIAL.println("Ligar Lamp");
      break;
    
    case 2:
      lamp.change_light_state();
      delay(20);
      break;
    
    default:
      USE_SERIAL.println("Mensagem mal formatada");
      break;
    }

    yield();
  }

  
}

//================================================================================================
// função feita para checar a flag do objeto lamp e saber se houve
//  mudança do estado da luz, com isso eh feita a publicação
//  da mensagem no topico "response" da confirmação

void publish_response(bool flag){
    
    if(flag){
      Serial.print("Mensagem será publicada: ");
      StaticJsonDocument<MAX_TAM_JSON> doc;
      doc["0"]= lamp.get_lightID();
      doc["device_type"] = DEVICE_TYPE_LIGHT_MODULE;
      doc["23"] = lamp.get_msg_id();
      lamp.clean_msg_id();
      //publicações de resposta de quando lampada desligada
      if(lamp.get_light_state()){
        doc["21"] = "0";
        doc["intensity"] = 0 ;
       if(lamp.get_time_on() != NULL){ 
          doc["time_on"] = lamp.get_time_on();
          StaticJsonDocument<64> docHourTopic;
          docHourTopic["time_on"] = lamp.get_time_on();
          
          char buffer_temp_hour[64];
          serializeJson(docHourTopic, buffer_temp_hour);
          Serial.println(buffer_temp_hour);
          
          mqttClient.publish(hour_on_topic.c_str(), buffer_temp_hour, true);
        }
      }
      //publicações de resposta de quando lampada ligada
      else{
        if(lamp.get_time_on() != NULL){ 
          doc["time_on"] = lamp.get_time_on();
        }
        doc["21"] = "1";
        delay(150);
        int light_value = analogRead(LIGHT_PIN);
        Serial.print("Light Value: ");
        Serial.println(light_value);
        doc["intensity"] = light_value ;
      }

      //Buffer usado para publicar a mensagem
      char buffer_temp[MAX_TAM_JSON];
      
      size_t n = serializeJson(doc, buffer_temp);
      Serial.println(buffer_temp);
      mqttClient.publish(response_topic, buffer_temp, n);
      yield();
      lamp.set_flag_response(false);
    }
     
}



//======== SETUP ==============================
void setup() {
  #ifdef DEBUG_ALL
    Serial.begin(115200);
   #endif
 //SETUP LIGHT SENSOR
    pinMode(LIGHT_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

// SETUP WIFI
  // wifiManager.resetSettings() ;
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
// SETUP TIME
  
 
   setup_time();  // sincroniza o relogio interno com o do servidor ntp
  
// SETUP MQTT 
    Serial.println("inicio config mqttCClient");
    //mqttClient.setWifi(&wifiClient);
//   SUBSCRIPITION
   
        // FIRMWARE MANAGEMENT TOPICS
    mqttClient.addSubscription(httpsUpload.c_str());
    mqttClient.addSubscription(reset_topic.c_str());
        //LIGHT CONTROLL TOPICS
    mqttClient.addSubscription(set_func_topic.c_str());
    mqttClient.addSubscription(set_func_room_topic.c_str());
    
    mqttClient.addSubscription( hour_on_topic.c_str() );
    
    

// SETUP LIGHT CONTROLL
    lamp.config();

    Serial.println("Setup Concluido");
    Serial.print("Versão do Firmware: ");
    Serial.println(firmware_version.c_str());
    mqttClient.printBufferSize();
    mqttClient.publish("5.light.2.ready", "{\"status\": \"on\"}");
    digitalWrite(LED_BUILTIN, LOW);
}
//====================================END SETUP

void loop() {
  // put your main code here, to run repeatedly:
  mqttClient.checkConnection();
  //Serial.println("Check connection");
  mqttClient.checkMsgs();
  //Serial.println("Check Msgs");
  publish_response(lamp.get_flag_response());
  //Serial.println("Check Response");
  
  yield();
  delay(50);
}
