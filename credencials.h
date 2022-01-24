#ifndef CREDENCIALS
#define CREDENCIALS
//==================================================================================================
//Configuração de Pinos na ESP32=====================================================================


  #define RELAY_PIN 4
  #define LED_BUILTIN 2
  #define LIGHT_PIN 34

//==================================================================================================
//==================================================================================================
//==================================================================================================
//IP do broker =====================================================================================

    const uint16_t mqtt_port = 1883;
    const char* mqtt_broker_ip = "andromeda.lasdpc.icmc.usp.br";
    

//==================================================================================================
//==================================================================================================
//==================================================================================================
//HTTP_UPDATER INFO ================================================================================
    
    const char* host = "andromeda.lasdpc.icmc.usp.br";
    const int httpsPort = 1880;
    const char* module = "light";
    const char* moduleURL = "light_module/";
    
    const char* firmware_version_char = "20200620001"; // versao atual: ano-mes-dia-versao do dia
    // Use web browser to view and copy
    // SHA1 fingerprint of the certificate
    const char* fingerprint = "F1 74 C4 35 69 B6 EE CF 5C 1D F6 96 24 17 50 F1 97 E2 31 A3";
    

//==================================================================================================
//==================================================================================================
//==================================================================================================
#endif
