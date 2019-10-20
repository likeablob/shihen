#include "display.h"
#include "config.h"

GxIO_SPI io1(SPI, PIN_DISP_CS, PIN_DISP_DC, -1); // CS, DC, RST(disabled)
GxEPD_Class display1(io1, -1, PIN_DISP_BUSY);    // RST(disabled), BUSY

void drawMessage(String msg) {
    uint16_t bg = GxEPD_BLACK;
    uint16_t fg = GxEPD_WHITE;
    int16_t x = DISP_HEIGHT / 2;
    int16_t y = DISP_WIDTH / 2;
    int16_t x1, y1;
    uint16_t w, h;

    uint8_t padding = 3;
    display1.setFont(&FreeMonoBold12pt7b);
    display1.getTextBounds(msg.c_str(), x, y, &x1, &y1, &w, &h);
    display1.fillRoundRect(x1 - w / 2 - padding, y1 - padding, w + padding * 2,
                           h + padding * 2, 1, bg);
    display1.setTextColor(fg);
    display1.setCursor(x - w / 2, y);
    display1.print(msg);
}

void drawToast(String msg, uint16_t fg, uint16_t bg) {
    int16_t x = DISP_HEIGHT - 60;
    int16_t y = DISP_WIDTH - 9;
    int16_t x1, y1;
    uint16_t w, h;
    // char buff[msg.length() + 1];
    // msg.toCharArray(buff, msg.length() + 1);

    uint8_t padding = 2;
    display1.setFont(&FreeMonoBold9pt7b);
    display1.getTextBounds(msg.c_str(), x, y, &x1, &y1, &w, &h);
    display1.fillRoundRect(x1 - padding, y1 - padding, w + padding * 2,
                           h + padding * 2, 1, bg);
    display1.setTextColor(fg);
    display1.setCursor(x, y);
    display1.print(msg);
}

img_read_err_t readImage(uint16_t pos, char **dest) {
    File f = storageService.openToRead("/offsets.bin");
    if(!f) {
        return IMG_READ_OPEN_FAILED;
    }
    // Here are N images. images[n] (0 < n < N-1)
    // offsets[0] = sizeOf(images[0]) // sizeOf 1st image
    // offsets[1] = sizeOf(images[0]) + sizeOf(images[1])
    // ...
    // offsets[N-1] = offsets[N-2] + sizeof(images[N-1])
    //
    // For n = 0:
    //  sizeOf(images[0]) = offsets[0]
    // For 0 < n <= N - 1:
    //  sizeOf(images[n]) = offsets[n-1] - offsets[n]
    //
    // Example A : N = 2
    //  sizeOf(images[0]) = offsets[0]
    //  sizeOf(images[1]) = offsets[1] - offsets[0]
    // Example B : N = 3
    //  sizeOf(images[0]) = offsets[0]
    //  sizeOf(images[1]) = offsets[1] - offsets[0]
    //  sizeOf(images[2]) = offsets[2] - offsets[1]
    //
    // sizeOf(offsets) = sizeOf(offset_t) * N
    typedef uint32_t offset_t;
    uint32_t offset1 = 0; // offsets[n-1]
    uint32_t offset2 = 0; // offsets[n]
    uint32_t offsetsSize = f.size();

    if(sizeof(offset_t) * pos >= offsetsSize) {
        return IMG_READ_TOO_LARGE_POS;
    }
    if(pos) {
        f.seek(sizeof(offset_t) * (pos - 1) % offsetsSize);
        f.read((uint8_t *)&offset1, sizeof(offset_t));
    }
    f.read((uint8_t *)&offset2, sizeof(offset_t));

    Serial.println("pos, f.size, offset1, offset2");
    Serial.println(pos);
    Serial.println(f.size());
    Serial.println(offset1);
    Serial.println(offset2);
    if(offset1 >= offset2) {
        return IMG_READ_INVALID_OFFSET;
    }
    f.close();

    // Decompress an image
    f = storageService.openToRead("/images.bin");
    if(!f) {
        Serial.printf("readfailed images: %d\n", f.size());
        return IMG_READ_OPEN_FAILED;
    }

    unsigned int cLen = offset2 - offset1;
    unsigned int dlen;
    char *source;
    int res;

    source = (char *)malloc(cLen);
    f.seek(offset1);
    f.readBytes(source, cLen);
    f.close();

    dlen = source[cLen - 1];
    dlen = 256 * dlen + source[cLen - 2];
    dlen = 256 * dlen + source[cLen - 3];
    dlen = 256 * dlen + source[cLen - 4];

    dlen++;

    *dest = (char *)malloc(dlen);

    struct uzlib_uncomp d;
    uzlib_uncompress_init(&d, NULL, 0);

    d.source = (uint8_t *)source;
    d.source_limit = (uint8_t *)source + cLen - 4;
    d.source_read_cb = NULL;

    res = uzlib_gzip_parse_header(&d);
    if(res != TINF_OK) {
        Serial.printf("Error parsing header: %d\n", res);
        free(source);
        free(*dest);
        return IMG_READ_DECOMPRESS_FAILED;
    }

    d.dest_start = d.dest = (uint8_t *)*dest;

    while(dlen) {
        unsigned int chunk_len = dlen < OUT_CHUNK_SIZE ? dlen : OUT_CHUNK_SIZE;
        d.dest_limit = d.dest + chunk_len;
        res = uzlib_uncompress_chksum(&d);
        dlen -= chunk_len;
        if(res != TINF_OK) {
            break;
        }
    }

    if(res != TINF_DONE) {
        Serial.printf("Error during decompression: %d\n", res);
        free(source);
        free(*dest);
        return IMG_READ_DECOMPRESS_FAILED;
    }

    Serial.printf("decompressed %lu bytes\n", d.dest - (uint8_t *)*dest);

    free(source);

    if(sizeof(offset_t) * (pos + 1) == offsetsSize) {
        return IMG_READ_OK_LAST_IMG;
    }
    return IMG_READ_OK;
}

img_read_err_t drawImage(uint16_t pos) {
    char *buff;
    img_read_err_t res;
#ifdef USE_BWR_PANEL
    res = readImage(pos, &buff);
    if(res <= IMG_READ_OK_LAST_IMG) {
        display1.drawBitmap((uint8_t *)buff, 0, 0, DISP_HEIGHT, DISP_WIDTH,
                            GxEPD_BLACK, display1.bm_partial_update);
        free(buff);
    } else {
        return res;
    }
    res = readImage(pos + 1, &buff);
    if(res <= IMG_READ_OK_LAST_IMG) {
        display1.drawBitmap(
            (uint8_t *)buff, 0, 0, DISP_HEIGHT, DISP_WIDTH, GxEPD_RED,
            display1.bm_partial_update | display1.bm_transparent);
        free(buff);
    } else {
        return res;
    }
    return res;
#endif
    res = readImage(pos, &buff);
    if(res <= IMG_READ_OK_LAST_IMG) {
        display1.drawBitmap((uint8_t *)buff, 0, 0, DISP_HEIGHT, DISP_WIDTH,
                            GxEPD_BLACK, display1.bm_partial_update);
        free(buff);
    }
    return res;
}