#ifndef TEMPO_H
#define TEMPO_H

#define DATE_STAMP_SIZE 20

class Tempo{
  private:
    char* _ntpServer { "pool.ntp.br" };
    long  _gmtOffset_sec { -10800 };
    int   _daylightOffset_sec { 0 };

  public:
    Tempo();
    virtual void setNtpServer(char *server);
    virtual void setGmtOffsetSec(long offset);
    virtual void setDaylightOffsetSec(int offset);
    virtual char* getNtpServer();
    virtual long getGmtOffsetSec();
    virtual int getDaylightOffsetSec();
};

void getDateStamp(char buffer[DATE_STAMP_SIZE]);
void printSerialTimeStamp();
void setup_time();

#endif
