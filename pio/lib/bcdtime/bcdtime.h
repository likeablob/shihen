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

#ifndef __TIMECONV_H
#define __TIMECONV_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// type of time data
//
// #ifndef TIME_H
// typedef uint32_t time_t;		// UTC unix epoch 0 == 1970/1/1.0
// #endif
typedef uint32_t date_t;		// Modified Julian Date (MJD) 0 == 1858/11/17.0
typedef uint32_t bcdtime_t;
typedef uint32_t bcddate_t;

struct bcddatetime_t {
    bcddate_t date;
    bcdtime_t time;
};

//
// bcd hex convert
//
extern uint8_t dtob (const uint8_t);
extern uint8_t btod (const uint8_t);
extern uint16_t wdtob (const uint16_t);
extern uint16_t wbtod (const uint16_t);

//
// bcd date/time to
//
extern time_t bcdToEpoch (const bcddate_t, const bcdtime_t);
extern time_t bcdDateTimeToEpoch (const bcddatetime_t);

//
// bcd date to
//
extern date_t bcdDateToMjd (const bcddate_t);
extern uint8_t bcdDateToWeekday (const bcddate_t);

//
// bcd time to
//
extern time_t bcdTimeToSeconds (const bcdtime_t);
extern String bcdTimeToTimeString (const bcdtime_t);

//
// mjd to
//
extern bcddate_t mjdToBcdDate (const date_t);
extern uint8_t mjdToWeekday (const date_t);
extern time_t mjdToEpoch (const date_t);

//
// epoch to
//
extern date_t epochToMjd (const time_t);
extern bcddatetime_t epochToBcd (const time_t);
extern bcddate_t epochToBcdDate (const time_t);
extern bcdtime_t epochToBcdTime (const time_t);
extern bcdtime_t epochToBcdDayTime (const time_t);
extern uint8_t epochToWeekday (const time_t);
extern String epochToTimeString (const time_t);

#ifdef __cplusplus
}
#endif

#endif

// end of header
