#include "tempo.hpp"
#include "time.h"
#include <Arduino.h>

Tempo::Tempo(){
/*  _ntpServer = "pool.ntp.org";
  _gmtOffset_sec = -10800;
  _daylightOffset_sec = 0;*/
};

void Tempo::setNtpServer(char *server){ _ntpServer = server; }
void Tempo::setGmtOffsetSec(long offset){ _gmtOffset_sec = offset; }
void Tempo::setDaylightOffsetSec(int offset){ _daylightOffset_sec = offset; }
char* Tempo::getNtpServer(){ return _ntpServer; }
long Tempo::getGmtOffsetSec(){ return _gmtOffset_sec; }
int Tempo::getDaylightOffsetSec(){ return _daylightOffset_sec; }

void getDateStamp(char buffer[DATE_STAMP_SIZE]){
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void setup_time(){
  Tempo tempo;
  Serial.print("Trying to configure local time from ntp server...");
  configTime(tempo.getGmtOffsetSec(), tempo.getDaylightOffsetSec(), tempo.getNtpServer());
  Serial.println(" Done!");

  Serial.print(F("\nWaiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    //Serial.println(now);
    now = time(nullptr);
  }
  Serial.println(" done!");
}

void printSerialTimeStamp(){
  char dateStamp[DATE_STAMP_SIZE];
  getDateStamp(dateStamp);
  Serial.print(dateStamp);
  Serial.print(" ");
}
