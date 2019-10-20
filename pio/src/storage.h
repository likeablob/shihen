#pragma once

#include "Arduino.h"
#include "config.h"

#ifdef USE_SD
#include <SD.h>
#include <SPI.h>
#elif defined USE_SPIFFS
#include "file-server.h"
#include <FS.h>
#endif

class StorageService {
  private:
    /* data */
  public:
    bool begin();
    File openToRead(const char *path);
};

extern StorageService storageService;