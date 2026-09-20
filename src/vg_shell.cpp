#include "VanGadget.h"

float vg_clamp(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

lv_obj_t *vg_page() {
  lv_obj_t *p = lv_obj_create(lv_scr_act());
  lv_obj_set_size(p, LCD_WIDTH, LCD_HEIGHT - STATUS_H);
  lv_obj_align(p, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(p, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(p, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(p, 0, 0);
  lv_obj_set_style_pad_all(p, 0, 0);
  lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
  return p;
}

lv_obj_t *vg_txt(lv_obj_t *p, const char *t, const lv_font_t *f, lv_coord_t y) {
  lv_obj_t *o = lv_label_create(p);
  lv_label_set_text(o, t);
  lv_obj_set_style_text_font(o, f, 0);
  lv_obj_set_style_text_color(o, lv_color_white(), 0);
  lv_obj_align(o, LV_ALIGN_TOP_MID, 0, y);
  return o;
}

static void on_icon(lv_event_t *e) {
  show_app((int)(intptr_t)lv_event_get_user_data(e));
}

lv_obj_t *vg_icon(lv_obj_t *parent, const char *title, lv_color_t col, int appId, int colx, int row) {
  const int bw = 148;
  const int bh = 100;
  const int gapx = 16;
  const int gapy = 14;
  const int grid_w = bw * 2 + gapx;
  const int ox = (LCD_WIDTH - grid_w) / 2 + colx * (bw + gapx);
  const int oy = 12 + row * (bh + gapy);

  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, bw, bh);
  lv_obj_set_pos(btn, ox, oy);
  lv_obj_set_style_radius(btn, 22, 0);
  lv_obj_set_style_bg_color(btn, col, 0);
  lv_obj_set_style_shadow_width(btn, 0, 0);
  lv_obj_set_style_pad_all(btn, 0, 0);
  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
  lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(btn, on_icon, LV_EVENT_PRESSED, (void *)(intptr_t)appId);

  lv_obj_t *cap = lv_label_create(btn);
  lv_label_set_text(cap, title);
  lv_obj_set_style_text_color(cap, lv_color_white(), 0);
  lv_obj_set_style_text_font(cap, &lv_font_montserrat_14, 0);
  lv_obj_center(cap);
  return btn;
}

void shell_build() {
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
  statusBar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(statusBar, LCD_WIDTH, STATUS_H);
  lv_obj_align(statusBar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(statusBar, lv_color_make(12, 12, 14), 0);
  lv_obj_set_style_border_width(statusBar, 0, 0);
  lv_obj_set_style_radius(statusBar, 0, 0);
  lv_obj_set_style_pad_all(statusBar, 0, 0);
  lv_obj_clear_flag(statusBar, LV_OBJ_FLAG_SCROLLABLE);

  sbTime = lv_label_create(statusBar);
  lv_label_set_text(sbTime, "--:--");
  lv_obj_set_style_text_font(sbTime, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(sbTime, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(sbTime, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(sbTime, lv_color_make(12, 12, 14), 0);
  lv_obj_set_width(sbTime, 80);
  lv_obj_align(sbTime, LV_ALIGN_LEFT_MID, 28, 0);
  sbBle = lv_label_create(statusBar);
  lv_label_set_text(sbBle, "Vantastic");
  lv_obj_set_style_text_font(sbBle, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(sbBle, lv_color_make(80, 80, 80), 0);
  lv_obj_align(sbBle, LV_ALIGN_CENTER, 0, 0);
  sbBatt = lv_label_create(statusBar);
  lv_label_set_text(sbBatt, "--%");
  lv_obj_set_style_text_font(sbBatt, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(sbBatt, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(sbBatt, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(sbBatt, lv_color_make(12, 12, 14), 0);
  lv_obj_set_width(sbBatt, 64);
  lv_obj_set_style_text_align(sbBatt, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_align(sbBatt, LV_ALIGN_RIGHT_MID, -28, 0);

  for (int i = 0; i < APP_COUNT; i++) page[i] = NULL;
  home_build();
  more_build();
  soon_build();
  level_build();
  batt_build();
  clock_build();
  eng_build();
  torch_build();
  sys_build();

  homeBar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(homeBar, 96, 5);
  lv_obj_align(homeBar, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_obj_set_style_radius(homeBar, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(homeBar, lv_color_make(180, 180, 180), 0);
  lv_obj_set_style_bg_opa(homeBar, LV_OPA_70, 0);
  lv_obj_set_style_border_width(homeBar, 0, 0);
  lv_obj_set_style_pad_all(homeBar, 0, 0);
  lv_obj_clear_flag(homeBar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

void shell_tick() {
  if (millis() - tUi < 500) return;
  tUi = millis();
  char s[24];
  if (haveRtc) {
    RTC_DateTime t = rtc.getDateTime();
    snprintf(s, sizeof(s), "%02d:%02d", t.getHour(), t.getMinute());
    lv_label_set_text(sbTime, s);
  }
  if (havePmu) {
    snprintf(s, sizeof(s), "%d%%", pmu.getBatteryPercent());
    lv_label_set_text(sbBatt, s);
  }
  lv_obj_set_style_text_color(sbBle, bleWanted ? lv_color_make(90, 180, 255) : lv_color_make(80, 80, 80), 0);
}
