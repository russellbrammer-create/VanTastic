#include "VanGadget.h"

static lv_obj_t *toneLvlLbl;

static void tone_lvl_caption() {
  const char *n[] = { "", "Low", "Med", "High" };
  char s[24];
  snprintf(s, sizeof(s), "Level tone: %s", n[toneLevel]);
  lv_label_set_text(toneLvlLbl, s);
}

static void on_tone_lvl(lv_event_t *) {
  audio_set_level(toneLevel >= 3 ? 1 : toneLevel + 1);
  tone_lvl_caption();
}

void sys_build() {
  page[APP_SYS] = vg_page();
  vg_txt(page[APP_SYS], "System", &lv_font_montserrat_20, 16);

  lv_obj_t *btn = lv_btn_create(page[APP_SYS]);
  lv_obj_set_size(btn, LCD_WIDTH - 48, 48);
  lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_set_style_bg_color(btn, lv_color_make(50, 50, 60), 0);
  lv_obj_add_event_cb(btn, on_tone_lvl, LV_EVENT_CLICKED, NULL);
  toneLvlLbl = lv_label_create(btn);
  lv_obj_set_style_text_color(toneLvlLbl, lv_color_white(), 0);
  lv_obj_center(toneLvlLbl);
  tone_lvl_caption();

  sysBody = vg_txt(page[APP_SYS],
                   "More settings later",
                   &lv_font_montserrat_14, 140);
}

void sys_tick() {}
