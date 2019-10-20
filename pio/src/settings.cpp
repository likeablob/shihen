#include "settings.h"

DeviceSettings deviceSettings;
SettingsService settingsService;

void SettingsService::begin() { EEPROM.begin(512); }

void SettingsService::loadBootCount() {
    uint8_t bootCount = EEPROM.read(ADDR_BOOT_COUNT);
    bootCount++;
    deviceSettings.bootCount = bootCount;
    EEPROM.write(ADDR_BOOT_COUNT, bootCount);
    EEPROM.commit();
}

void SettingsService::loadSettings() {
    EEPROM.get(ADDR_DEVICE_SETTINGS, deviceSettings);
    uint16_t imageId = deviceSettings.imageId;
    deviceSettings.imageId = imageId;
}

void SettingsService::saveSettings() {
    EEPROM.put(ADDR_DEVICE_SETTINGS, deviceSettings);
    EEPROM.commit();
}