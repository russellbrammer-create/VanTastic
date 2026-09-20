#include "VanGadget.h"

static lv_obj_t *battAmp;
static lv_obj_t *battBar;
static int lastSocDrawn = -1;
static int lastV = -1, lastA = -9999;
static bool lastLink = false;

static void bar_col(int soc) {
  lv_color_t c;
  if (soc < 20) c = lv_color_make(220, 50, 50);
  else if (soc < 50) c = lv_color_make(230, 160, 40);
  else c = lv_color_make(40, 200, 90);
  lv_obj_set_style_bg_color(battBar, c, LV_PART_INDICATOR);
}

void batt_build() {
  page[APP_BATT] = vg_page();
  vg_txt(page[APP_BATT], "Leisure  Minty", &lv_font_montserrat_14, 8);

  battPct = vg_txt(page[APP_BATT], "--%", &lv_font_montserrat_28, 36);
  lv_obj_set_width(battPct, LCD_WIDTH - 40);
  lv_obj_set_style_text_align(battPct, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_bg_opa(battPct, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(battPct, lv_color_black(), 0);

  battBar = lv_bar_create(page[APP_BATT]);
  lv_obj_set_size(battBar, LCD_WIDTH - 48, 28);
  lv_obj_align(battBar, LV_ALIGN_TOP_MID, 0, 88);
  lv_bar_set_range(battBar, 0, 100);
  lv_bar_set_value(battBar, 0, LV_ANIM_OFF);
  lv_obj_set_style_radius(battBar, 8, LV_PART_MAIN);
  lv_obj_set_style_radius(battBar, 8, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(battBar, lv_color_make(32, 32, 36), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(battBar, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(battBar, LV_OPA_COVER, LV_PART_INDICATOR);
  bar_col(0);

  battVolt = vg_txt(page[APP_BATT], "--.- V", &lv_font_montserrat_28, 132);
  lv_obj_set_width(battVolt, LCD_WIDTH - 40);
  lv_obj_set_style_text_align(battVolt, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_bg_opa(battVolt, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(battVolt, lv_color_black(), 0);

  battAmp = vg_txt(page[APP_BATT], "  +0.0 A", &lv_font_montserrat_28, 178);
  lv_obj_set_width(battAmp, LCD_WIDTH - 40);
  lv_obj_set_style_text_align(battAmp, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(battAmp, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_bg_opa(battAmp, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(battAmp, lv_color_black(), 0);
  lv_obj_set_style_pad_hor(battAmp, 8, 0);

  battState = vg_txt(page[APP_BATT], "searching", &lv_font_montserrat_14, 230);
}

void batt_tick() {
  char s[32];
  if (leisure_linked()) {
    if (leisure_soc() != lastSocDrawn) {
      snprintf(s, sizeof(s), "%d%%", leisure_soc());
      lv_label_set_text(battPct, s);
      lv_bar_set_value(battBar, leisure_soc(), LV_ANIM_OFF);
      bar_col(leisure_soc());
      lastSocDrawn = leisure_soc();
      if (statusBar) lv_obj_invalidate(statusBar);
    }
    int v = (int)(leisure_volt() * 100.0f);
    int a = (int)(leisure_amp() * 10.0f);
    if (v != lastV) {
      snprintf(s, sizeof(s), "%.2f V", leisure_volt());
      lv_label_set_text(battVolt, s);
      lastV = v;
    }
    if (a != lastA) {
      snprintf(s, sizeof(s), "%+6.1f A", leisure_amp());
      lv_label_set_text(battAmp, s);
      lastA = a;
    }
  } else if (lastLink) {
    lv_label_set_text(battPct, "--%");
    lv_bar_set_value(battBar, 0, LV_ANIM_OFF);
    lastSocDrawn = -1;
    lastV = -1;
    lastA = -9999;
  }
  lastLink = leisure_linked();
  lv_label_set_text(battState, leisure_msg());
}