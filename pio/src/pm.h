#pragma once
#include "config.h"

#include <Arduino.h>

#ifdef USE_RTC
#include "rtc.h"
#include <TimeClient.h>
#elif defined USE_TPL
// nothing to include
#endif

class PowerManagementService {
  private:
    /* data */
  public:
    void begin();
    void shutdown();
    void syncTime();
    bool readSw0();
};

extern PowerManagementService powerManagementService;
