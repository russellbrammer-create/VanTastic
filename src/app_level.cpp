#include "VanGadget.h"

extern volatile bool toneOn;
static lv_obj_t *noteLbl;

static void note_caption() {
  lv_label_set_text(noteLbl, toneOn ? "Tone on" : "Tone off");
}

static void on_note(lv_event_t *) {
  audio_set_enabled(!toneOn);
  note_caption();
}

static void bubble_col(bool on) {
  lv_obj_set_style_bg_color(bubble, on ? lv_color_make(0,255,255) : lv_color_make(60,230,60), 0);
  lv_obj_set_style_bg_grad_color(bubble, on ? lv_color_make(0,95,115) : lv_color_make(0,90,0), 0);
}

void level_build() {
  page[APP_LEVEL] = vg_page();
  lv_obj_set_style_bg_opa(page[APP_LEVEL], LV_OPA_COVER, 0);
  dial = lv_obj_create(page[APP_LEVEL]);
  lv_obj_set_size(dial, 240, 240);
  lv_obj_align(dial, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(dial, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(dial, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(dial, lv_color_make(70,70,70), 0);
  lv_obj_set_style_border_width(dial, 2, 0);
  lv_obj_set_style_outline_width(dial, 0, 0);
  lv_obj_set_style_radius(dial, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_clip_corner(dial, true, 0);
  lv_obj_set_style_pad_all(dial, 0, 0);
  lv_obj_set_style_shadow_width(dial, 0, 0);
  lv_obj_clear_flag(dial, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *h = lv_obj_create(dial);
  lv_obj_set_size(h, 200, 2);
  lv_obj_align(h, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_border_width(h, 0, 0);
  lv_obj_set_style_bg_color(h, lv_color_make(70,70,70), 0);
  lv_obj_clear_flag(h, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *v = lv_obj_create(dial);
  lv_obj_set_size(v, 2, 200);
  lv_obj_align(v, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_border_width(v, 0, 0);
  lv_obj_set_style_bg_color(v, lv_color_make(70,70,70), 0);
  lv_obj_clear_flag(v, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *ring = lv_obj_create(dial);
  lv_obj_set_size(ring, 64, 64);
  lv_obj_align(ring, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_color(ring, lv_color_make(120,120,120), 0);
  lv_obj_set_style_border_width(ring, 2, 0);
  lv_obj_clear_flag(ring, LV_OBJ_FLAG_SCROLLABLE);

  bubble = lv_obj_create(dial);
  lv_obj_set_size(bubble, 36, 36);
  lv_obj_align(bubble, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_radius(bubble, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_grad_dir(bubble, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_border_width(bubble, 2, 0);
  lv_obj_set_style_shadow_width(bubble, 0, 0);
  lv_obj_set_style_outline_width(bubble, 0, 0);
  lv_obj_clear_flag(bubble, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_FLOATING);
  bubble_col(false);

  lv_obj_t *note = lv_btn_create(page[APP_LEVEL]);
  lv_obj_set_size(note, 76, 76);
  lv_obj_align(note, LV_ALIGN_BOTTOM_RIGHT, -10, -58);
  lv_obj_set_style_radius(note, 20, 0);
  lv_obj_set_style_bg_color(note, lv_color_make(40, 40, 48), 0);
  lv_obj_set_style_shadow_width(note, 0, 0);
  lv_obj_clear_flag(note, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
  lv_obj_add_flag(note, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(note, on_note, LV_EVENT_PRESSED, NULL);
  noteLbl = lv_label_create(note);
  lv_obj_set_style_text_color(noteLbl, lv_color_white(), 0);
  lv_obj_center(noteLbl);
  note_caption();
}

void level_tick() {
  if (millis() - tImu < 30) return;
  if (!qmi.getDataReady() || !qmi.getAccelerometer(acc.x, acc.y, acc.z)) return;
  tImu = millis();
  float p = atan2f(acc.x, sqrtf(acc.y * acc.y + acc.z * acc.z)) * RAD_TO_DEG;
  float r = atan2f(acc.y, sqrtf(acc.x * acc.x + acc.z * acc.z)) * RAD_TO_DEG;
  if (!seeded) { pitch = p; roll = r; seeded = true; }
  else { pitch += (p - pitch) * 0.08f; roll += (r - roll) * 0.08f; }
  gTilt = fmaxf(fabsf(pitch), fabsf(roll));
  if (app != APP_LEVEL) return;
  int32_t x = lroundf(vg_clamp(roll, -20, 20) / 20.0f * 90);
  int32_t y = lroundf(vg_clamp(pitch, -20, 20) / 20.0f * 90);
  if (x != lastX || y != lastY) {
    lv_obj_invalidate(dial);
    lv_obj_align(bubble, LV_ALIGN_CENTER, x, -y);
    lv_obj_invalidate(dial);
    lastX = x; lastY = y;
  }
  bool on = fabsf(pitch) < 0.8f && fabsf(roll) < 0.8f;
  if (on != levelNow) { bubble_col(on); levelNow = on; }
}
