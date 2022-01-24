#include "httpUpdateHandler.hpp"
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "tempo.hpp"

void handleUpdate(const String& HTTP_updateUrl, uint8_t *nv, const String& firmware_version){
  HTTPUpdateResult result;
  WiFiClientSecure httpClient;
  bool keep_trying = true;
  int i;

  httpUpdate.rebootOnUpdate(false);
  String newVersion((char*)nv);
  if(strcmp((char*)nv, firmware_version.c_str()) != 0){
    printSerialTimeStamp();
    Serial.print("Current version: ");
    Serial.println(firmware_version);
    printSerialTimeStamp();
    Serial.print("New version available: ");
    Serial.println(newVersion);
    do{
      printSerialTimeStamp();
      Serial.println("Updating...");
      result = httpUpdate.update(httpClient, HTTP_updateUrl + newVersion, firmware_version);
      switch(result){
        case HTTP_UPDATE_OK:
          Serial.println("");
          Serial.println("");
          printSerialTimeStamp();
          Serial.println("Updated succesfully.");
          printSerialTimeStamp();
          Serial.println("It has been an honor to serve you, now my new me will take control. Good bye :D");
          printSerialTimeStamp();
          Serial.println("Restarting in ");
          for(i = 0; i < 5; i++){
            Serial.print(5 - i);
            Serial.println(" seconds");
            delay(1000);
          }
          ESP.restart();
          break;
        case HTTP_UPDATE_FAILED:
          printSerialTimeStamp();
          Serial.print("Update failed... Error: (");
          Serial.print(httpUpdate.getLastError());
          Serial.print("): ");
          Serial.println(httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          printSerialTimeStamp();
          Serial.println("There was no update to be done.");
          keep_trying = false;
          break;
      }
      delay(1000);
    }while(keep_trying);
  }
  else{
    printSerialTimeStamp();
    Serial.print("Version ");
    Serial.print(newVersion);
    Serial.println(" is already in use.");
  }
}
