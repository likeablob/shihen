#include <Arduino.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
WiFiManager wifiManager;
ESP8266WebServer server(80);

#include <Chrono.h>

extern "C" {
#include "user_interface.h"
}

#include "config.h"
#include "display.h"
#include "ota.h"
#include "pm.h"
#include "rtc.h"
#include "settings.h"
#include "storage.h"
#include "utils.h"

void showModeInfo(String title) {
    display1.fillScreen(GxEPD_WHITE);
    display1.setTextColor(GxEPD_BLACK);
    display1.setFont(&FreeMonoBold18pt7b);
    display1.setCursor(2, 24 + 2);
    display1.println(title);
    display1.setFont(&FreeMonoBold9pt7b);

    display1.setCursor(0, 24 + 18 + 4);
    display1.print(" Image id:");
    display1.printf("%d", deviceSettings.imageId);
    display1.println();

    // display1.print(" Boot count:");
    // display1.printf("%d%%%d", deviceSettings.bootCount,
    //                           deviceSettings.refreshInterval);

#ifdef USE_RTC
    display1.print(" Time:");
    bcddatetime_t bcddatetime = RTC.now();
    Serial.print(bcddatetime.date, HEX);
    Serial.write(' ');
    Serial.print(bcdTimeToTimeString(bcddatetime.time));
    Serial.println();
    Serial.println(bcddatetime.time, HEX);
    display1.print(bcdTimeToTimeString(bcddatetime.time));
    display1.println();
#endif

    // display1.print(" repo:");
    // display1.print(deviceSettings.repository);
    // display1.println();
    //
    // display1.print(" orientation:");
    // display1.print(deviceSettings.orientation);
    // display1.println();

    float batV, batVFraction;
    getBatteryVoltage(&batV, &batVFraction);

    display1.print(" Vbat:");
    display1.printf("%.2fV", batV);
    display1.printf("(%d%%)", (int)(batVFraction * 100));
    display1.println();

    display1.print(" AP:");
    String apName = WiFi.SSID();
    display1.print(apName.length() ? apName : DEVICE_NAME);
    display1.println();

    display1.print(" IP:");
    display1.print(WiFi.localIP());
    display1.println();
}

void startWiFiManager() {
    showModeInfo("AP Mode");
    display1.updateWindow(1, 1, DISP_HEIGHT - 2, DISP_WIDTH - 2);
    wifiManager.setTimeout(120);
    WiFiManagerParameter orientation("Orientation", "3", "3", 2);
    WiFiManagerParameter refreshInterval("Refresh interval(N*2h)", "6", "6", 2);
    wifiManager.addParameter(&orientation);
    wifiManager.addParameter(&refreshInterval);
    if(!wifiManager.startConfigPortal(DEVICE_NAME)) {
        Serial.println("Timeout! Failed to connect");
        drawMessage("Timeout");
        display1.updateWindow(1, 1, DISP_HEIGHT - 2, DISP_WIDTH - 2);
        powerManagementService.shutdown();
    }

    deviceSettings.orientation = atoi(orientation.getValue());
    deviceSettings.refreshInterval = atoi(refreshInterval.getValue());
    settingsService.saveSettings();
}

uint32_t otaStart = 0;
void setup() {
    WiFi.forceSleepBegin();
    Serial.begin(115200);
    Serial.println("setup");

    powerManagementService.begin();

    settingsService.begin();
    settingsService.loadBootCount();
    settingsService.loadSettings();
    //     uint8_t sw0StateA = readSw0();
    // if (bootCount % 6 != 0 && !sw0StateA) {
    //   shutdown();
    // }

#if BOARD_REVISION == 6
    pinMode(PIN_DISP_RST, OUTPUT);
    digitalWrite(PIN_DISP_RST, LOW);
    delay(10);
    digitalWrite(PIN_DISP_RST, HIGH);
    delay(10);
#endif

    display1.init();
    display1.setRotation(3);

    bool storageInitialized = storageService.begin();
#ifdef USE_SD
    if(!storageInitialized) {
        Serial.print("SD init failed");
        drawMessage("SD init failed");
        delay(1000);
        ESP.reset();
    }
#endif

    // Read battery voltage
    float batV, batVFraction;
    getBatteryVoltage(&batV, &batVFraction);

    //  Read and draw an image
    uint16_t imageId = deviceSettings.imageId;
    img_read_err_t readRes = drawImage(imageId);
    Serial.print("readRes:");
    Serial.println(readRes);
    if(readRes <= IMG_READ_OK_LAST_IMG) {
        display1.drawFastHLine(0, DISP_WIDTH - 1, batVFraction * DISP_HEIGHT,
                               GxEPD_BLACK);
        display1.update();

#ifdef USE_BWR_PANEL
        if(readRes == IMG_READ_OK_LAST_IMG) {
            imageId = 0;
        } else {
            imageId += 2;
            deviceSettings.imageId = imageId; // next imageId
        }
#else
        if(readRes == IMG_READ_OK_LAST_IMG) {
            imageId = 0;
        } else {
            imageId++;
        }
        while(powerManagementService.readSw0()) { // fast forwarding
            readRes = drawImage(imageId++);
            if(readRes >= IMG_READ_OK_LAST_IMG) {
                imageId = 0;
            }
            if(readRes > IMG_READ_OK_LAST_IMG) {
                break;
            }
            display1.updateWindow(
                1, 1, DISP_HEIGHT - 2,
                DISP_WIDTH - 2); // FIXME: workaround for partial updating
            if(!powerManagementService.readSw0()) {
                display1.drawFastHLine(0, DISP_WIDTH - 1,
                                       batVFraction * DISP_HEIGHT, GxEPD_BLACK);
                display1.update();
                delay(500);
                break;
            }
        }
        deviceSettings.imageId = imageId; // next imageId
#endif
    } else {
        deviceSettings.imageId = 0;
    }

    settingsService.saveSettings();

    bool sw0StateB = powerManagementService.readSw0();
    if(sw0StateB) {
        WiFi.forceSleepWake();
        delay(1);
        WiFi.mode(WIFI_STA);
        wifi_station_connect();
        WiFi.begin();
        drawMessage("OTAMode");
        display1.updateWindow(1, 1, DISP_HEIGHT - 2,
                              DISP_WIDTH -
                                  2); // FIXME: workaround for partial updating

        int16_t count = 100;
        while(--count) { // Wait connection for 10 sec at max
            delay(100);
            if(WiFi.isConnected()) {
                break;
            }
        }
        if(!count) {
            Serial.println("startWifiManager");
            startWiFiManager();
        }

        deviceSettings.imageId = 0;
        settingsService.saveSettings();

        powerManagementService.syncTime();
        setupOTA();
#ifdef USE_SPIFFS
        fileServer.begin(&server);
#endif
        server.begin();
        showModeInfo("OTA Mode");
        display1.updateWindow(0, 0, DISP_HEIGHT - 1, DISP_WIDTH - 1);
    } else {
        powerManagementService.shutdown();
    }
    otaStart = millis();
}

Chrono refreshTask;
#define PUSH_DURATION_SHORT 3000 // 3 sec
#define PUSH_DURATION_LONG 5000  // 5 sec
void loop() {
    static uint16_t refreshCount = 0;
    static uint32_t pushStart = 0;

    handleOTA();
    server.handleClient();

    if(!refreshCount && powerManagementService.readSw0()) {
        return; // skip the following until the switch is released
    }

    if(refreshTask.hasPassed(3000)) {
        refreshTask.restart();
        refreshCount++;
        String msg = "Btn>";
        if(pushStart > 0) {
            uint32_t pushDuration = millis() - pushStart;
            if(pushDuration < PUSH_DURATION_SHORT) {
                msg = "Exit";
            } else if(PUSH_DURATION_LONG < pushDuration) {
                msg = "Reset";
            }
        }

        showModeInfo("OTA Mode");
        uint16_t bg = refreshCount % 2 ? GxEPD_BLACK : GxEPD_WHITE;
        uint16_t fg = refreshCount % 2 ? GxEPD_WHITE : GxEPD_BLACK;
        drawToast(msg, fg, bg);

#ifdef USE_BWR_PANEL
        // FIXME: no way to update due to slow refresh rate
#else
        display1.updateWindow(1, 1, DISP_HEIGHT - 2, DISP_WIDTH - 2);
#endif
    }

    // read the switch's state
    if(pushStart == 0 && powerManagementService.readSw0()) {
        pushStart = millis();
    } else if(pushStart > 0 && !powerManagementService.readSw0()) {
        uint32_t pushDuration = millis() - pushStart;
        if(pushDuration < PUSH_DURATION_SHORT) {
            drawMessage("Exitting..");
            display1.updateWindow(1, 1, DISP_HEIGHT - 2, DISP_WIDTH - 2);
            delay(500);
            display1.eraseDisplay(true);
            powerManagementService.shutdown();
        } else if(PUSH_DURATION_LONG < pushDuration) {
            drawMessage("Resetting");
            display1.updateWindow(1, 1, DISP_HEIGHT - 2, DISP_WIDTH - 2);
            wifiManager.resetSettings();
            powerManagementService.shutdown();
        }
        pushStart = 0;
    }
}
