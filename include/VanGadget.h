#pragma once
#include <lvgl.h>
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "lv_conf.h"
#include "SensorQMI8658.hpp"
#include "SensorPCF85063.hpp"
#include "XPowersLib.h"
#include "HWCDC.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

enum AppId {
  APP_HOME = 0,
  APP_LEVEL,
  APP_BATT,
  APP_CLOCK,
  APP_ENG,
  APP_TORCH,
  APP_SYS,
  APP_MORE,
  APP_SOON,
  APP_COUNT
};

#define STATUS_H 52
#define DOCK_ZONE 56

extern HWCDC USBSerial;
extern SensorQMI8658 qmi;
extern SensorPCF85063 rtc;
extern XPowersAXP2101 pmu;
extern IMUdata acc;
extern Arduino_CO5300 *gfx;
extern std::unique_ptr<Arduino_IIC> TP;

extern lv_obj_t *statusBar, *sbTime, *sbBatt, *sbBle, *homeBar;
extern lv_obj_t *home;
extern lv_obj_t *page[APP_COUNT];
extern lv_obj_t *dial, *bubble;
extern lv_obj_t *battPct, *battVolt, *battState;
extern lv_obj_t *clkTime, *clkDate, *clkTemp;
extern lv_obj_t *engTemp, *engHint;
extern lv_obj_t *sysBody;

extern volatile int app;
extern bool havePmu, haveRtc, seeded, levelNow, torchOn;
extern float pitch, roll;
extern uint32_t tImu, tUi;
extern int32_t lastX, lastY;
extern uint8_t brightness;
extern bool bleWanted;
extern volatile float gTilt;
extern int toneLevel;   // 1 low, 2 med, 3 high
extern volatile bool toneOn;
extern bool audioOk;

extern float leisureVolt;
extern float leisureAmp;
extern int leisureSoc;
extern bool leisureLinked;
extern char leisureMsg[32];

void ble_begin();
void ble_tick();

void audio_begin();
void audio_set_level(int level);
void audio_set_enabled(bool on);

float vg_clamp(float v, float a, float b);
lv_obj_t *vg_page();
lv_obj_t *vg_txt(lv_obj_t *p, const char *t, const lv_font_t *f, lv_coord_t y);
lv_obj_t *vg_icon(lv_obj_t *parent, const char *title, lv_color_t col, int appId, int colx, int row);
void show_app(int id);

void home_build();
void more_build();
void soon_build();
void level_build();
void level_tick();
void batt_build();
void batt_tick();
void clock_build();
void clock_tick();
void eng_build();
void eng_tick();
void torch_build();
void torch_tick();
void sys_build();
void sys_tick();
void shell_build();
void shell_tick();
