#include "VanGadget.h"

void clock_build() {
  page[APP_CLOCK] = vg_page();
  clkTime = vg_txt(page[APP_CLOCK], "--:--", &lv_font_montserrat_28, 80);
  lv_obj_set_width(clkTime, LCD_WIDTH - 48);
  lv_obj_set_style_text_align(clkTime, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(clkTime, LV_LABEL_LONG_CLIP);

  clkDate = vg_txt(page[APP_CLOCK], "---- -- --", &lv_font_montserrat_14, 150);
  lv_obj_set_width(clkDate, LCD_WIDTH - 48);
  lv_obj_set_style_text_align(clkDate, LV_TEXT_ALIGN_CENTER, 0);

  clkTemp = vg_txt(page[APP_CLOCK], "cabin -- C", &lv_font_montserrat_14, 200);
  lv_obj_set_width(clkTemp, LCD_WIDTH - 48);
  lv_obj_set_style_text_align(clkTemp, LV_TEXT_ALIGN_CENTER, 0);
}

void clock_tick() {
  char s[24];
  if (haveRtc) {
    RTC_DateTime t = rtc.getDateTime();
   snprintf(s, sizeof(s), "%02d:%02d", t.getHour(), t.getMinute());
lv_label_set_text(clkTime, s);
snprintf(s, sizeof(s), "%04d-%02d-%02d", t.getYear(), t.getMonth(), t.getDay());
lv_label_set_text(clkDate, s);
  } else {
    lv_label_set_text(clkTime, "no RTC");
  }
  if (havePmu) {
    snprintf(s, sizeof(s), "board %.0f C", pmu.getTemperature());
    lv_label_set_text(clkTemp, s);
  }
}
