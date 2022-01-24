#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

    #include <Arduino.h>
    #include <ArduinoJson.h>
    #include "mqttSetup.hpp"
   
    #ifndef MAX_TAM_JSON 
        #define MAX_TAM_JSON 512
    #endif

    #ifndef DEVICE_TYPE_LIGHT_MODULE
        #define DEVICE_TYPE_LIGHT_MODULE 2
    #endif
   
    class Light_Control{
        private:
            int _pin_relay { 0 };
            bool _light_state { true };
            int _lightID {0} ;
            bool _flag_response { false };
            double _time_on = 0;
            time_t _last_time_ON = NULL;
            char* _msg_id;
            
            
        public:
            Light_Control(int pin_relay, bool light_state, int lightID);
            virtual void config();
            virtual void set_pin_relay(int pin_relay);
            virtual void set_light_state(bool state);
            virtual bool get_light_state();
            virtual void change_light_state();
            virtual int  get_lightID();
            virtual double get_time_on();
            virtual char* get_msg_id();
            virtual void set_msg_id(const char* msg_id);
            virtual void clean_msg_id();
            //virtual void check_response(MqttSetup MQTTclient, const char* response_topic);
            virtual bool get_flag_response();
            virtual void set_flag_response(bool flag);
            virtual void update_time_on(uint8_t* timeJson);
            
        
    };

#endif
