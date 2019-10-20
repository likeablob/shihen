#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#define ADDR_BOOT_COUNT 0
#define ADDR_DEVICE_SETTINGS 1

class DeviceSettings {
  public:
    uint8_t orientation = 3;
    uint16_t bootCount = 0;
    uint16_t imageId = 0;
    uint8_t refreshInterval = 1;
};

extern DeviceSettings deviceSettings;

class SettingsService {
  public:
    void begin();
    void loadBootCount();
    void loadSettings();
    void saveSettings();
};

extern SettingsService settingsService;
