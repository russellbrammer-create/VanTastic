#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <memory>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "SensorQMI8658.hpp"
#include "SensorPCF85063.hpp"
#include "XPowersLib.h"
#include "HWCDC.h"

extern HWCDC USBSerial;
extern SensorQMI8658 qmi;
extern SensorPCF85063 rtc;
extern XPowersAXP2101 pmu;
extern IMUdata acc;
extern Arduino_CO5300 *gfx;
extern std::unique_ptr<Arduino_IIC> TP;

extern bool havePmu, haveRtc;
extern uint8_t brightness;