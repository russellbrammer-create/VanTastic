#include "VanGadget.h"

void home_build() {
  home = vg_page();
  page[APP_HOME] = home;
  vg_icon(home, "Level",   lv_color_make(40, 160, 80),  APP_LEVEL, 0, 0);
  vg_icon(home, "Leisure", lv_color_make(40, 120, 200), APP_BATT,  1, 0);
  vg_icon(home, "Clock",   lv_color_make(200, 140, 40), APP_CLOCK, 0, 1);
  vg_icon(home, "Engine",  lv_color_make(200, 70, 50),  APP_ENG,   1, 1);
  vg_icon(home, "Torch",   lv_color_make(220, 200, 70), APP_TORCH, 0, 2);
  vg_icon(home, "System",  lv_color_make(90, 90, 100),  APP_SYS,   1, 2);
}
