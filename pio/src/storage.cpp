#include "storage.h"

StorageService storageService;

bool StorageService::begin() {
#ifdef USE_SD
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);
    return SD.begin(PIN_SD_CS);
#elif defined USE_SPIFFS
    return SPIFFS.begin();
#endif
}

File StorageService::openToRead(const char *path) {
#ifdef USE_SD
    File f = SD.open(path, FILE_READ);
#elif defined USE_SPIFFS
    File f = SPIFFS.open(path, "r");
#endif
    return f;
}