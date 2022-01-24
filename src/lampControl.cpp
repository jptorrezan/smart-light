#include "lampControl.hpp"

Light_Control::Light_Control(int pin_relay, bool light_state, int lightID){
    this->_pin_relay = pin_relay;
    this->_light_state = light_state;
    this->_lightID = lightID;
}

void Light_Control::config(){
    pinMode(this->_pin_relay, OUTPUT);
    digitalWrite(this->_pin_relay, this->_light_state);
}

bool Light_Control::get_light_state(){
    return this->_light_state;
}

double Light_Control::get_time_on(){
    return this->_time_on;
}

int Light_Control::get_lightID(){
    return this->_lightID;
}

void Light_Control::set_pin_relay(int pin_relay){
    this->_pin_relay = pin_relay;
}

void Light_Control::set_light_state(bool state){
    this->_light_state = state;
    digitalWrite(this->_pin_relay, this->_light_state);
    this->_flag_response = true;
    
    //Caso lampada estiver ligada, pegará o horario e atualizará a variável que marca o tempo em que a lamp foi ligada
    time_t time_buffer;
    
    if(this->_light_state == false){ 
      Serial.println("Report ON:"); 
      if(!time(&time_buffer)){
        Serial.println("Failed to obtain time");
      }
      Serial.print("time_buffer: ");
      Serial.println(time_buffer);
      if (this->_last_time_ON == NULL){
        this->_last_time_ON = time_buffer;
      }
      else if( difftime(time_buffer, this->_last_time_ON ) < 0 ){
        this->_last_time_ON = time_buffer;
      }
      Serial.print("_last_time_on: ");
      Serial.println(this->_last_time_ON);
      
        
     //CASO SEJA COMANDO OFF
     //(executa a subtração do tempo atual, com o tempo em que a lampada foi ligada) 
    }else{
      time(&time_buffer);
      Serial.println("Report OFF:");
      if(this->_last_time_ON != NULL){
        double dif_seconds;
        dif_seconds = difftime(time_buffer, this->_last_time_ON);
        Serial.print("Time atual: ");
        Serial.println(time_buffer);
        Serial.println("_time_on: ");
        Serial.println(this->_time_on);
        
        this->_time_on += (dif_seconds)/3600;
        Serial.print("Tempo total: ");
        Serial.println(this->_time_on);
      }
      this->_last_time_ON = NULL;
      Serial.println("_last_time_ON zerado.");

      }
}

void Light_Control::change_light_state(){
    if(this->_light_state){
        this->set_light_state(false);
    }
    else{
        this->set_light_state(true);
    } 
    this->_flag_response = true;      
}

bool Light_Control::get_flag_response(){
  return this->_flag_response;
}

void Light_Control::set_flag_response(bool flag){
  this->_flag_response = flag;
}

void Light_Control::update_time_on(uint8_t* timeJson){
 
  StaticJsonDocument<MAX_TAM_JSON> doc;   
  DeserializationError error = deserializeJson(doc, timeJson); //payload transformado em JSON
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());                  //flag de erro na deserialização
    return;
  }
  this->_time_on = (doc["time_on"]);
  
  Serial.print("Report update_time_on");
  Serial.print("_time_on: ");
  Serial.println(this->_time_on);
}

char* Light_Control::get_msg_id(){
  return _msg_id;
}
void Light_Control::set_msg_id(const char* msg_id){
  _msg_id = (char*)msg_id;
}
void Light_Control::clean_msg_id(){
  _msg_id = "";
}
