/***************
 *
 * bcdtime - BCD, MJD, Epoch date time convert functions
 *
 * target architectures: Atmel AVR (ATmega 328P, 1284P and other)
 *
 * release site: https://github.com/askn37/Futilities
 * maintainer: askn https://twitter.com/askn37
 *
 */

#include <Arduino.h>
#include "bcdtime.h"

//
// 8bit decimal integer to bcd 4+4bit hexdecimal
//
// 0-99 to 0x00-0x99 (159 to 0xF9)
//
uint8_t dtob (const uint8_t dint) {
    return (((dint / 10) << 4) + (dint % 10));
}

//
// bcd 4+4bit hexdecimal to 8bit decimal integer
//
// 0x00-0x99 to 0-99 (0xF9 to 159)
//
uint8_t btod (const uint8_t bcd) {
    return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

//
// 16bit decimal integer to bcd 4x4bit hexdecimal
//
// 0-9999 to 0x0000-0x9999 (15999 to 0xF999)
//
uint16_t wdtob (const uint16_t dint) {
    return (((dint / 1000) << 12)
          + (((dint / 100) % 10) << 8)
          + (((dint / 10) % 10) << 4)
          + (dint % 10));
}

//
// bcd 4x4bit hexdecimal to 16bit decimal integer
//
// 0x0000-0x9999 to 0-9999 (0xF999 to 15999)
//
uint16_t wbtod (const uint16_t bcd) {
    return ((bcd >> 12) * 1000
          + ((bcd >> 8) & 0x0F) * 100
          + ((bcd >> 4) & 0x0F) * 10
          + (bcd & 0x0F));
}

//
// bcd date to integer mjd
//
// 0x18581117 to 0
// 0x19700101 to 2973483
//
date_t bcdDateToMjd (const bcddate_t bcd_date) {
    uint8_t t_day = btod(bcd_date);
    uint8_t t_mon = btod(bcd_date >> 8);
    uint16_t t_year = wbtod(bcd_date >> 16);
    if (t_mon < 3) { t_year--; t_mon += 12; }
    return ((36525L * t_year / 100)
        + (t_year / 400) - (t_year / 100)
        + (3059L * (t_mon - 2) / 100)
        + t_day - 678912L);
}

//
// bcd time to integer seconds
//
// 0x000000-0x235959 to 0-86399
//
time_t bcdTimeToSeconds (const bcdtime_t bcd_time) {
    uint8_t t_sec  = btod(bcd_time & 0x7F) % 60;
    uint8_t t_min  = btod((bcd_time >> 8) & 0x7F) % 60;
    uint8_t t_hour = btod((bcd_time >> 16) & 0x3F) % 24;
    uint8_t t_day  = btod(bcd_time >> 24);
    return (86400L * t_day + 3600L * t_hour + 60L * t_min + t_sec);
}

//
// mjd to bcd date
//
// 0       to 0x18581117
// 2973483 to 0x19700101
//
bcddate_t mjdToBcdDate (const date_t mjd) {
    int32_t jdn = mjd + 678881L;
    uint32_t t_base = (jdn << 2)
                    + ((((((jdn + 1) << 2)
                    / 146097 + 1) * 3) >> 2) << 2) + 3;
    uint32_t t_leap = ((t_base % 1461) >> 2) * 5 + 2;

    uint32_t t_year = t_base / 1461;
    uint32_t t_mon = t_leap / 153 + 3;
    uint32_t t_day = (t_leap % 153) / 5 + 1;
    if (t_mon > 12) { t_year++; t_mon -= 12; }

    return (dtob(t_day)
        + ((uint32_t)dtob(t_mon) << 8)
        + ((uint32_t)wdtob(t_year) << 16));
}

//
// mjd to epoch
//
// 1970/01/01 to 0
// 2038/01/19 to 0x80000000
// 2106/02/07 to 0xFFFFFFFF
//
time_t mjdToEpoch (const date_t mjd) {
    return bcdToEpoch(mjdToBcdDate(mjd), 0);
}

//
// bcd date/time to epoch
//
// 0x19701010,0x000000 to 0
//
time_t bcdToEpoch (const bcddate_t bcd_date, const bcdtime_t bcd_time) {
    return (86400 * (bcdDateToMjd(bcd_date) - 40587L)
                   + bcdTimeToSeconds(bcd_time));
}
time_t bcdDateTimeToEpoch (const bcddatetime_t bcddatetime) {
    return (86400 * (bcdDateToMjd(bcddatetime.date) - 40587L)
                   + bcdTimeToSeconds(bcddatetime.time));
}

//
// epoch to bcd date
//
// 0 to 0x19700101
//
bcddate_t epochToBcdDate (const time_t t_epoch) {
    return mjdToBcdDate(t_epoch / 86400 + 40587L);
}

bcdtime_t epochToBcdTime (const time_t t_epoch) {
    return (((uint32_t)dtob(t_epoch / 3600 % 24) << 16)
          + ((uint16_t)dtob(t_epoch / 60 % 60) << 8)
          + dtob(t_epoch % 60));
}

bcdtime_t epochToBcdDayTime (const time_t t_epoch) {
    return (((uint32_t)dtob(t_epoch / 86400) << 24)
          + ((uint32_t)dtob(t_epoch / 3600 % 24) << 16)
          + ((uint16_t)dtob(t_epoch / 60 % 60) << 8)
          + dtob(t_epoch % 60));
}

uint8_t epochToWeekday (const time_t t_epoch) {
    return ((t_epoch / 86400 + 4) % 7);
}

date_t epochToMjd (const time_t t_epoch) {
    return (t_epoch / 86400 + 40587L);
}

bcddatetime_t epochToBcd (const time_t t_epoch) {
    bcddatetime_t bcddatetime = {
        epochToBcdDate(t_epoch),
        epochToBcdTime(t_epoch)
    };
    return bcddatetime;
}

uint8_t mjdToWeekday (const date_t mjd) {
    return ((mjd + 3) % 7);
}

uint8_t bcdDateToWeekday (const bcddate_t bcd_date) {
    return ((bcdDateToMjd(bcd_date) + 3) % 7);
}

String epochToTimeString (const time_t t_epoch) {
    uint32_t t_time = t_epoch % 86400U;
    t_time = t_time % 60
           + (t_time / 60) % 60 * 100
           + (t_time / 3600) % 24 * 10000
           + 1000000L;
    String str = String(t_time, DEC);
    return str.substring(str.length() - 6);
}

String bcdTimeToTimeString (const bcdtime_t bcd_time) {
    String str = String(bcd_time | 0x1000000, HEX);
    return str.substring(str.length() - 6);
}

// end of code
