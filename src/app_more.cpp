#include "VanGadget.h"

void more_build() {
  page[APP_MORE] = vg_page();
  vg_icon(page[APP_MORE], "Radio",  lv_color_make(80, 90, 160),  APP_SOON, 0, 0);
  vg_icon(page[APP_MORE], "Notes",  lv_color_make(70, 130, 120), APP_SOON, 1, 0);
  vg_icon(page[APP_MORE], "Maps",   lv_color_make(50, 110, 90),  APP_SOON, 0, 1);
  vg_icon(page[APP_MORE], "Fuel",   lv_color_make(180, 90, 40),  APP_SOON, 1, 1);
  vg_icon(page[APP_MORE], "Winch",  lv_color_make(120, 80, 50),  APP_SOON, 0, 2);
  vg_icon(page[APP_MORE], "Garage", lv_color_make(90, 90, 90),   APP_SOON, 1, 2);
}

void soon_build() {
  page[APP_SOON] = vg_page();
  vg_txt(page[APP_SOON], "Not fitted yet", &lv_font_montserrat_14, 120);
}
