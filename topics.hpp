#ifndef TOPICS_H
#define TOPICS_H
#include<string> 
#include<Arduino.h>
using namespace std;

//================================================================================================================
//DECLARAÇAO DOS CAMPOS DOS TOPICOS ==============================================================================

    // lightID = ID da lampada (terceiro campo do topico)
    const int lightID = 2;
    #define LIGHTIDSTRING "2"

    //ID do ambiente
    //(primeiros campo dos tópicos)
    const char* room = "5";

    //caractere que separa os campos
    string separator = ".";

    
//================================================================================================================
// Operaçoes de contrução de topicos =============================================================================   
    
    //aux strings
    string firmware = "firmware/";
    string update = "update/";
    string state = "state";
    string set_state = "state";
    string change_state = "state";
    string ready = "ready";
    string reset = "reset/";
    string hour = "hour";

    //converter
    string moduleString(module);
    string roomString(room);
    string lightIDString = LIGHTIDSTRING;

    //historico de horas online
    string hour_on_topic = roomString + separator + moduleString + separator + lightIDString + separator + hour ;

    

    

                ///<PUBLISH TOPICS>
    
    //Topico de resposta das ações 
    //"response"   
        const char* response_topic = "response";

            ///<SUBSCRIBE TOPICS>

    //Topico responsável pela atualização do firmware da ESP        
    //"firmware/update/light"
        string httpsUpload = firmware + update + moduleString ;
    
    //Topico responsável por reiniciar a ESP
    //"firmware/reset/light/2"
        string reset_topic = firmware + reset + moduleString +"/"+ lightIDString;

    //Topico para retornar o estado da lamp
    //"4.light.2.state"
        string get_topic = roomString +separator+ moduleString +separator + lightIDString + separator + set_state;
    
    //Topico em que serão publicados os comandos de ligar/desligar lamp
    //"4.light.2"
        string set_func_topic = roomString +separator+ moduleString +separator + lightIDString ;

    //Topico em que serão publicados os comandos de ligar/desligar lamp porem para todas as lamp do ambiente 
    //"4.light"
        string set_func_room_topic = roomString + separator + moduleString ;



#endif
