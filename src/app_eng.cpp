#include "VanGadget.h"

static lv_obj_t *engT2;
static lv_obj_t *engRpm;
static lv_obj_t *engPsi;
static int lastT1 = -9999, lastT2 = -9999, lastRpm = -1, lastP = -1;
static bool lastLink = false;

static lv_obj_t *line(lv_obj_t *p, const char *t, lv_coord_t y) {
  lv_obj_t *l = vg_txt(p, t, &lv_font_montserrat_16, y);
  lv_obj_set_width(l, LCD_WIDTH - 32);
  lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
  return l;
}

void eng_build() {
  page[APP_ENG] = vg_page();
  vg_txt(page[APP_ENG], "Engine", &lv_font_montserrat_16, 12);

  engTemp = line(page[APP_ENG], "T1  --.- C",  56);
  engT2   = line(page[APP_ENG], "T2  --.- C",  96);
  engRpm  = line(page[APP_ENG], "RPM ----",    136);
  engPsi  = line(page[APP_ENG], "Oil --.- kPa",176);
  engHint = line(page[APP_ENG], "waiting for VanSense", 230);
  lv_obj_set_style_text_color(engHint, lv_color_make(140, 140, 140), 0);

  lastT1 = lastT2 = lastRpm = lastP = -9999;
  lastLink = false;
}

void eng_tick() {
  char s[40];
  bool on = sense_linked();
  if (on != lastLink) {
    lv_label_set_text(engHint, on ? "VanSense live" : "waiting for VanSense");
    lastLink = on;
    if (!on) {
      lastT1 = lastT2 = lastRpm = lastP = -9999;
      return;
    }
  }
  if (!on) return;

  int t1 = (int)(sense_temp1() * 10.0f);
  int t2 = (int)(sense_temp2() * 10.0f);
  int rpm = (int)sense_revs();
  int p = (int)(sense_press() * 10.0f);

  if (t1 != lastT1) {
    snprintf(s, sizeof(s), "T1  %.1f C", sense_temp1());
    lv_label_set_text(engTemp, s);
    lastT1 = t1;
  }
  if (t2 != lastT2) {
    snprintf(s, sizeof(s), "T2  %.1f C", sense_temp2());
    lv_label_set_text(engT2, s);
    lastT2 = t2;
  }
  if (rpm != lastRpm) {
    snprintf(s, sizeof(s), "RPM %d", rpm);
    lv_label_set_text(engRpm, s);
    lastRpm = rpm;
  }
  if (p != lastP) {
    snprintf(s, sizeof(s), "Oil %.1f kPa", sense_press());
    lv_label_set_text(engPsi, s);
    lastP = p;
  }
}