#include "rtc.h"

RTC_PCF8563 RTC;

void initAlarm(void) {
    bcddatetime_t bcddatetime = RTC.now();
    time_t mm = bcdTimeToSeconds(bcddatetime.time) + 60 * 2;
    date_t alarm = epochToBcdTime(mm);

    uint32_t flags = WAKE_TIME_TABLE;
    uint8_t currentHour = btod((alarm >> 16) & 0xFF);
    // uint8_t currentMinute = btod((alarm >> 8) & 0xFF);

    uint8_t nextHour = 0;
    bool isFound = false;
    for(size_t i = currentHour + 1; i < 24; i++) { // search forward
        if((flags >> i) & 0x1) {
            nextHour = i;
            isFound = true;
            break;
        }
    }
    if(!isFound) { // search backward
        for(size_t i = 0; i <= currentHour; i++) {
            if((flags >> i) & 0x1) {
                nextHour = i;
                isFound = true;
                break;
            }
        }
    }

    date_t _alarm = ((dtob(nextHour) & 0xFF) << 24) // hour
                    | ((0x0 & 0xFF) << 8)           // minutes
                    // | (((alarm >> 8)&0xFF) << 8) // minutes
                    | 0x00FF00FF;
    Serial.println(currentHour, HEX);
    Serial.println(nextHour, HEX);
    Serial.println(alarm, HEX);
    Serial.println((alarm >> 16) & 0xFF, HEX);
    Serial.println(_alarm, HEX);

    if(RTC.setAlarm(_alarm) && RTC.activeAlarm(true)) {
        date_t alarm = RTC.getAlarm();
    }
}
