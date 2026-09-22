#include "VanGadget.h"

static lv_obj_t *engT2;
static lv_obj_t *engOut;
static lv_obj_t *engRpm;
static lv_obj_t *engVbat;
static int lastT1 = -9999, lastT2 = -9999, lastOut = -9999;
static int lastRpm = -1, lastV = -1;
static bool lastLink = false;
static uint8_t lastWarn = 0;

static lv_obj_t *line(lv_obj_t *p, const char *t, lv_coord_t y) {
  lv_obj_t *l = vg_txt(p, t, &lv_font_montserrat_16, y);
  lv_obj_set_width(l, LCD_WIDTH - 32);
  lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
  return l;
}

void eng_build() {
  page[APP_ENG] = vg_page();
  vg_txt(page[APP_ENG], "Engine", &lv_font_montserrat_16, 8);

  engTemp = line(page[APP_ENG], "Block  --.- C",  44);
  engT2   = line(page[APP_ENG], "Cooler --.- C",  80);
  engOut  = line(page[APP_ENG], "Out    --.- C", 116);
  engRpm  = line(page[APP_ENG], "RPM    ----",   152);
  engVbat = line(page[APP_ENG], "Start  --.-- V",188);
  engHint = line(page[APP_ENG], "waiting for VanSense", 240);
  lv_obj_set_style_text_color(engHint, lv_color_make(140, 140, 140), 0);

  lastT1 = lastT2 = lastOut = lastRpm = lastV = -9999;
  lastLink = false;
  lastWarn = 0;
}

void eng_tick() {
  char s[40];
  bool on = sense_linked();
  uint8_t warn = 0;
  if (on && sense_frost()) warn |= 1;
  if (on && sense_overrev()) warn |= 2;
  if (on && sense_dummy()) warn |= 4;

  if (on != lastLink || warn != lastWarn) {
    if (!on) {
      lv_label_set_text(engHint, "waiting for VanSense");
      lv_obj_set_style_text_color(engHint, lv_color_make(140, 140, 140), 0);
    } else if (warn & 2) {
      lv_label_set_text(engHint, "OVER-REV");
      lv_obj_set_style_text_color(engHint, lv_color_make(220, 60, 60), 0);
    } else if (warn & 1) {
      lv_label_set_text(engHint, "FROST");
      lv_obj_set_style_text_color(engHint, lv_color_make(90, 180, 255), 0);
    } else if (warn & 4) {
      lv_label_set_text(engHint, "VanSense dummy");
      lv_obj_set_style_text_color(engHint, lv_color_make(200, 160, 40), 0);
    } else {
      lv_label_set_text(engHint, "VanSense live");
      lv_obj_set_style_text_color(engHint, lv_color_make(140, 140, 140), 0);
    }
    lastLink = on;
    lastWarn = warn;
    if (!on) {
      lastT1 = lastT2 = lastOut = lastRpm = lastV = -9999;
      return;
    }
  }
  if (!on) return;

  int t1 = (int)(sense_temp1() * 10.0f);
  int t2 = (int)(sense_temp2() * 10.0f);
  int to = (int)(sense_temp_out() * 10.0f);
  int rpm = (int)sense_revs();
  int v = (int)(sense_vbat() * 100.0f);

  if (t1 != lastT1) {
    snprintf(s, sizeof(s), "Block  %.1f C", sense_temp1());
    lv_label_set_text(engTemp, s);
    lastT1 = t1;
  }
  if (t2 != lastT2) {
    snprintf(s, sizeof(s), "Cooler %.1f C", sense_temp2());
    lv_label_set_text(engT2, s);
    lastT2 = t2;
  }
  if (to != lastOut) {
    snprintf(s, sizeof(s), "Out    %.1f C", sense_temp_out());
    lv_label_set_text(engOut, s);
    lastOut = to;
  }
  if (rpm != lastRpm) {
    snprintf(s, sizeof(s), "RPM    %d", rpm);
    lv_label_set_text(engRpm, s);
    lv_obj_set_style_text_color(engRpm,
        sense_overrev() ? lv_color_make(220, 60, 60) : lv_color_white(), 0);
    lastRpm = rpm;
  }
  if (v != lastV) {
    snprintf(s, sizeof(s), "Start  %.2f V", sense_vbat());
    lv_label_set_text(engVbat, s);
    lastV = v;
  }
}
