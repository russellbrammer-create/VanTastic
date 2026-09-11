#include "VanGadget.h"

void torch_build() {
  page[APP_TORCH] = vg_page();
  lv_obj_set_style_bg_color(page[APP_TORCH], lv_color_white(), 0);
  lv_obj_t *tl = lv_label_create(page[APP_TORCH]);
  lv_label_set_text(tl, "flick up to home");
  lv_obj_set_style_text_color(tl, lv_color_make(40, 40, 40), 0);
  lv_obj_align(tl, LV_ALIGN_BOTTOM_MID, 0, -16);
}

void torch_tick() {
  if (!torchOn) {
    torchOn = true;
    gfx->setBrightness(255);
  }
}
