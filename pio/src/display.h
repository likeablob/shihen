#pragma once

#include <Arduino.h>
#include <SPI.h>

#include <GxEPD.h>

#ifdef USE_BWR_PANEL
#include <GxGDEW029Z10/GxGDEW029Z10.h> // 2,9inch b/w/r
#else
#include <GxGDEH029A1/GxGDEH029A1.h> // 2,9inch b/w
#endif

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

#include "storage.h"

#include <uzlib.h>
#define OUT_CHUNK_SIZE 1

extern GxEPD_Class display1;

void drawMessage(String msg);
void drawToast(String msg, uint16_t fg, uint16_t bg);

typedef enum {
    IMG_READ_OK = 0,
    IMG_READ_OK_LAST_IMG = 1,
    IMG_READ_OPEN_FAILED,
    IMG_READ_TOO_LARGE_POS,
    IMG_READ_INVALID_OFFSET,
    IMG_READ_DECOMPRESS_FAILED,

} img_read_err_t;

img_read_err_t readImage(uint16_t pos, char **dest);
img_read_err_t drawImage(uint16_t pos);
