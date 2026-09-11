#include "VanGadget.h"

void eng_build() {
  page[APP_ENG] = vg_page();
  vg_txt(page[APP_ENG], "Engine", &lv_font_montserrat_16, 24);
  engTemp = vg_txt(page[APP_ENG], "-- C", &lv_font_montserrat_28, 90);
  engHint = vg_txt(page[APP_ENG], "waiting for ESP sensor", &lv_font_montserrat_14, 160);
}

void eng_tick() {
  lv_label_set_text(engTemp, "-- C");
  lv_label_set_text(engHint, "waiting for ESP sensor");
}
