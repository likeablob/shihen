#pragma once
#ifndef INCLUDE_GUARD_CONFIG_H
#define INCLUDE_GUARD_CONFIG_H

/* Configuration flags */
#define USE_SD
// #define USE_SPIFFS

#define USE_RTC
// #define USE_TPL

// #define USE_BWR_PANEL

/* Other */
#define TIME_ZONE 9
// WAKE_TIME_TABLE, bitN : Wake up flag for N o'clock
// e.g 0x400001 : Wake up at AM 00:00 and PM 10:00 : (1 << 22 | 1)
// e.g 0xFFFFFF : Wake up at AM 00:00 and PM 11:00
#define WAKE_TIME_TABLE 0xFFFFFF
#define DISP_WIDTH 128
#define DISP_HEIGHT 296

#define BOARD_REVISION 6
#if BOARD_REVISION == 5
#define BOARD_REVISION_STR "rev.5"
#elif BOARD_REVISION == 6
#define BOARD_REVISION_STR "rev.6"
#endif

/* PIN definitions */
#if BOARD_REVISION == 5
#define PIN_SW0_SENS 4

#define PIN_TPL_DONE 5

#define PIN_DISP_MOSI 13 // not used
#define PIN_DISP_MISO 12 // not used
#define PIN_DISP_SCK 14  // not used
#define PIN_DISP_CS 15
#define PIN_DISP_DC 0
#define PIN_DISP_BUSY 16
#define PIN_DISP_RST 2

#define PIN_SD_MOSI 13 // not used
#define PIN_SD_MISO 12 // not used
#define PIN_SD_SCK 14  // not used
#define PIN_SD_CS 3    // UART RX

#define PIN_RTC_SDA 2
#define PIN_RTC_SCL 5
#elif BOARD_REVISION == 6
#define PIN_SW0_SENS 10

#define PIN_TPL_DONE 5

#define PIN_DISP_MOSI 13 // not used
#define PIN_DISP_MISO 12 // not used
#define PIN_DISP_SCK 14  // not used
#define PIN_DISP_CS 15
#define PIN_DISP_DC 0
#define PIN_DISP_BUSY 16
#define PIN_DISP_RST 4

#define PIN_SD_MOSI 13 // not used
#define PIN_SD_MISO 12 // not used
#define PIN_SD_SCK 14  // not used
#define PIN_SD_CS 3    // UART RX

#define PIN_RTC_SDA 2
#define PIN_RTC_SCL 5

#endif

#endif // INCLUDE_GUARD_CONFIG_H