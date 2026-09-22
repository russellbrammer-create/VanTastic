#pragma once
#include <Arduino.h>
#include <BLEAdvertisedDevice.h>

extern bool bleWanted;

extern int toneLevel;
extern volatile bool toneOn;
extern bool audioOk;
extern bool torchOn;

void ble_begin();
void ble_tick();

void audio_begin();
void audio_set_level(int level);
void audio_set_enabled(bool on);

void  imu_tick();
float tilt_deg();
float tilt_pitch();
float tilt_roll();

bool        leisure_linked();
int         leisure_soc();
float       leisure_volt();
float       leisure_amp();
const char *leisure_msg();

void  sense_on_advert(BLEAdvertisedDevice *d);
void  sense_tick();
bool  sense_linked();
float sense_temp1();
float sense_temp2();
float sense_revs();
float sense_press();
float sense_temp_out();
float sense_vbat();
bool  sense_frost();
bool  sense_overrev();
bool  sense_dummy();
