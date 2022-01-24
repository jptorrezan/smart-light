#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
class wifiSetup {
  private:
    const char *_ssid;
    const char *_password;

  public:
    wifiSetup();
    void connect(const char *ssid, const char *password);
    void reconnect();
    void checkConnection();
    void macAddress(char buffer[18]);
    IPAddress localIP();
};

#endif
