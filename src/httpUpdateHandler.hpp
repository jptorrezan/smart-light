#ifndef HTTP_UPDATE_HANDLER_H
#define HTTP_UPDATE_HANDLER_H
#include <Arduino.h>

void handleUpdate(const String& HTTP_updateUrl, uint8_t *nv, const String& firmware_version);

#endif
