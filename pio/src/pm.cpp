#include "pm.h"

#ifdef USE_RTC
TimeClient timeClient(TIME_ZONE);
#endif

PowerManagementService powerManagementService;

void PowerManagementService::begin() {
#ifdef USE_TPL
    pinMode(PIN_SW0_SENS, INPUT_PULLDOWN_16);

    pinMode(PIN_TPL_DONE, OUTPUT);
    digitalWrite(PIN_TPL_DONE, LOW);
#else
    pinMode(PIN_SW0_SENS, INPUT_PULLUP);

    Wire.begin(PIN_RTC_SDA, PIN_RTC_SCL);
    while(!RTC.begin()) {
        Serial.println(F("RTC not ready"));
        delay(10);
    }
    RTC.setTimer(
        0xF003); // will be triggered immediately to keep the device awake
    RTC.activeTimer(true);
#endif
}

void PowerManagementService::shutdown() {
    Serial.println("shutting down");
#ifdef USE_TPL
    for(size_t i = 0; i < 100; i++) {
        digitalWrite(PIN_TPL_DONE, HIGH);
        delay(1);
        digitalWrite(PIN_TPL_DONE, LOW);
        delay(1);
    }
#else
    Wire.begin(PIN_RTC_SDA, PIN_RTC_SCL);

    while(!RTC.begin()) {
        Serial.println(F("RTC not ready"));
        delay(100);
    }
    RTC.activeAlarm(false);
    initAlarm();
    RTC.activeAlarm(true);
    RTC.activeTimer(false);
#endif

    ESP.restart();
}

void PowerManagementService::syncTime() {
#ifdef USE_RTC
    timeClient.updateTime();
    RTC.adjust((time_t)timeClient.getCurrentEpochWithUtcOffset());

#endif
}

bool PowerManagementService::readSw0() {
#ifdef USE_TPL
    return digitalRead(PIN_SW0_SENS);
#else
    return !digitalRead(PIN_SW0_SENS);
#endif
}