# VanTastic / VanGadget

Board: Waveshare-style ESP32-S3 AMOLED (CO5300 / CST816 / QMI8658 / AXP2101 / PCF85063 / ES8311).
Build: VS Code + PlatformIO + pioarduino (Arduino-ESP32 3.x). Official espressif32 2.0.17 will not compile GFX 1.6.

Must keep:
- include/pin_config.h, include/lv_conf.h, include/VanGadget.h
- src/es8311.c + include/es8311.h
- lib/Arduino_DriveBus* (not on registry)
- SensorLib 0.4: rtc.begin(Wire, SDA, SCL); RTC fields via getHour() etc.

Windows: long paths + PLATFORMIO_CORE_DIR=C:\pio if pioarduino unpack fails.

Repo: https://github.com/russellbrammer-create/VanTastic
Tag target: works-on-desk