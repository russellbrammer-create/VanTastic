#include "VanGadget.h"

static lv_obj_t *envOut;
static lv_obj_t *envHpa;
static lv_obj_t *envHint;
static int lastOut = -9999;
static int lastHpa = -1;
static bool lastLink = false;

void env_build() {
  page[APP_ENV] = vg_page();
  vg_txt(page[APP_ENV], "Enviro", &lv_font_montserrat_16, 8);

  envOut = vg_txt(page[APP_ENV], "--.- C", &lv_font_montserrat_48, 56);
  vg_txt(page[APP_ENV], "outside", &lv_font_montserrat_14, 112);

  envHpa = vg_txt(page[APP_ENV], "---- hPa", &lv_font_montserrat_36, 160);
  vg_txt(page[APP_ENV], "pressure", &lv_font_montserrat_14, 206);

  envHint = vg_txt(page[APP_ENV], "waiting for VanSense", &lv_font_montserrat_14, 250);
  lv_obj_set_style_text_color(envHint, lv_color_make(140, 140, 140), 0);

  lastOut = lastHpa = -9999;
  lastLink = false;
}

void env_tick() {
  char s[24];
  bool on = sense_linked();
  if (on != lastLink) {
    lv_label_set_text(envHint, on ? "VanSense" : "waiting for VanSense");
    lastLink = on;
    if (!on) {
      lastOut = lastHpa = -9999;
      lv_label_set_text(envOut, "--.- C");
      lv_label_set_text(envHpa, "---- hPa");
      return;
    }
  }
  if (!on) return;

  int o = (int)(sense_temp_out() * 10.0f);
  if (o != lastOut) {
    snprintf(s, sizeof(s), "%.1f C", sense_temp_out());
    lv_label_set_text(envOut, s);
    lastOut = o;
  }

  float h = sense_hpa();
  int hi = (int)(h * 10.0f);
  if (hi != lastHpa) {
    if (h >= 300.0f && h <= 1100.0f) {
      snprintf(s, sizeof(s), "%.0f hPa", h);
    } else {
      snprintf(s, sizeof(s), "---- hPa");
    }
    lv_label_set_text(envHpa, s);
    lastHpa = hi;
  }
}
