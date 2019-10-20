# SHIHEN/firmware
It's built on top of [esp8266/Arduino](https://github.com/esp8266/Arduino).

## Prerequisites
- [PlatformIO](https://platformio.org/)

## Build
- `Ctrl+Alt+B` in your editor, or 
```bash
$ pio run
```

## Upload
- `Ctrl + Alt + U` in your editor, or 
```bash
$ pio run -t upload
```
- ***Tips:*** Don't forget to comment out the lines below to do ISP(UART) programming.
```bash
# platform.ini
upload_port = ${env:main.device_name}.local
upload_protocol = espota
```
### ISP port (J1) pin definitions:
```
1: 5V (LDO_IN)
2: ESP_RESET: RST pin
3: ESP_BOOT: GPIO0 pin
4: GND
5: ESP_TXD: TX pin
6: ESP_RXD: RX pin
```

### Acknowledgments
- Please refer to
  - [platformio.ini](./platformio.ini).
  - [lib/](./lib).
