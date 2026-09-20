#include "VanGadget.h"


#if LV_FONT_MONTSERRAT_48
#define CLK_FONT_BIG &lv_font_montserrat_48
#else
#define CLK_FONT_BIG &lv_font_montserrat_28
#endif
#define CLK_FONT_SET &lv_font_montserrat_28

static lv_obj_t *btnRow;
static bool setting = false;
static bool holding = false;
static uint32_t holdStart = 0;
static uint32_t lastAct = 0;
static int editH = 0, editM = 0;
static int lastShownH = -1, lastShownM = -1;

bool clock_setting() { return setting; }

static void show_hm(int h, int m) {
  char s[16];
  snprintf(s, sizeof(s), "%02d:%02d", h, m);
  lv_label_set_text(clkTime, s);
}

static void load_rtc() {
  if (!haveRtc) return;
  RTC_DateTime t = rtc.getDateTime();
  editH = t.getHour();
  editM = t.getMinute();
}

static void save_rtc() {
  if (!haveRtc) return;
  RTC_DateTime t = rtc.getDateTime();
  rtc.setDateTime(t.getYear(), t.getMonth(), t.getDay(), editH, editM, 0);
  lastShownH = editH;
  lastShownM = editM;
}

static void apply_display_style() {
  lv_obj_set_style_text_font(clkTime, CLK_FONT_BIG, 0);
  lv_obj_set_style_text_color(clkTime, lv_color_white(), 0);
  lv_obj_add_flag(btnRow, LV_OBJ_FLAG_HIDDEN);
}

static void apply_set_style() {
  lv_obj_set_style_text_font(clkTime, CLK_FONT_BIG, 0);
  lv_obj_set_style_text_color(clkTime, lv_color_make(90, 200, 255), 0);
  lv_obj_clear_flag(btnRow, LV_OBJ_FLAG_HIDDEN);
}

static void enter_set() {
  setting = true;
  holding = false;
  holdStart = 0;
  lastAct = millis();
  load_rtc();
  show_hm(editH, editM);
  apply_set_style();
}

static void leave_set(bool save) {
  if (save) save_rtc();
  setting = false;
  holding = false;
  holdStart = 0;
  lastShownH = lastShownM = -1;
  apply_display_style();
}

void clock_on_flick() {
  if (setting) leave_set(true);
}

static void bump_hour(lv_event_t *) {
  lastAct = millis();
  editH++;
  if (editH > 23) editH = 0;
  show_hm(editH, editM);
}

static void bump_min(lv_event_t *) {
  lastAct = millis();
  editM++;
  if (editM > 59) {
    editM = 0;
    editH++;
    if (editH > 23) editH = 0;
  }
  show_hm(editH, editM);
}

static void on_press(lv_event_t *) {
  if (setting) return;
  holding = true;
  holdStart = millis();
}

static void on_release(lv_event_t *) {
  holding = false;
  holdStart = 0;
}

static lv_obj_t *make_btn(lv_obj_t *parent, const char *txt, lv_color_t col,
                          lv_event_cb_t cb, lv_coord_t x) {
  lv_obj_t *b = lv_btn_create(parent);
  lv_obj_set_size(b, 140, 72);
  lv_obj_set_pos(b, x, 0);
  lv_obj_set_style_bg_color(b, col, 0);
  lv_obj_set_style_shadow_width(b, 0, 0);
  lv_obj_set_style_radius(b, 18, 0);
  lv_obj_clear_flag(b, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);
  lv_obj_add_flag(b, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(b, cb, LV_EVENT_PRESSED, NULL);
  lv_obj_t *l = lv_label_create(b);
  lv_label_set_text(l, txt);
  lv_obj_set_style_text_color(l, lv_color_white(), 0);
  lv_obj_set_style_text_font(l, &lv_font_montserrat_14, 0);
  lv_obj_center(l);
  return b;
}

void clock_build() {
  page[APP_CLOCK] = vg_page();
  lv_obj_add_flag(page[APP_CLOCK], LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(page[APP_CLOCK], LV_OBJ_FLAG_GESTURE_BUBBLE);
  lv_obj_add_event_cb(page[APP_CLOCK], on_press, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(page[APP_CLOCK], on_release, LV_EVENT_RELEASED, NULL);

  clkTime = vg_txt(page[APP_CLOCK], "--:--", CLK_FONT_BIG, 80);
  lv_obj_set_width(clkTime, LCD_WIDTH - 16);
  lv_obj_set_height(clkTime, 100);
  lv_obj_set_style_text_align(clkTime, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(clkTime, LV_LABEL_LONG_CLIP);
  lv_obj_clear_flag(clkTime, LV_OBJ_FLAG_CLICKABLE);

  clkDate = vg_txt(page[APP_CLOCK], "---- -- --", &lv_font_montserrat_14, 200);
  lv_obj_set_width(clkDate, LCD_WIDTH - 48);
  lv_obj_set_style_text_align(clkDate, LV_TEXT_ALIGN_CENTER, 0);

  clkTemp = vg_txt(page[APP_CLOCK], "", &lv_font_montserrat_14, 240);
  lv_obj_set_width(clkTemp, LCD_WIDTH - 48);
  lv_obj_set_style_text_align(clkTemp, LV_TEXT_ALIGN_CENTER, 0);

  btnRow = lv_obj_create(page[APP_CLOCK]);
  lv_obj_set_size(btnRow, LCD_WIDTH - 16, 80);
  lv_obj_align(btnRow, LV_ALIGN_TOP_MID, 0, 280);
  lv_obj_set_style_bg_opa(btnRow, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(btnRow, 0, 0);
  lv_obj_set_style_pad_all(btnRow, 0, 0);
  lv_obj_clear_flag(btnRow, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_GESTURE_BUBBLE);

  const int bw = 140, gap = 16;
  const int x0 = ((LCD_WIDTH - 16) - (bw * 2 + gap)) / 2;
  make_btn(btnRow, "Hour", lv_color_make(200, 140, 40), bump_hour, x0);
  make_btn(btnRow, "Min",  lv_color_make(40, 140, 180), bump_min,  x0 + bw + gap);
  lv_obj_add_flag(btnRow, LV_OBJ_FLAG_HIDDEN);
}

void clock_tick() {
  if (!setting && holding && (millis() - holdStart >= 10000)) {
    enter_set();
  }
  if (setting && (millis() - lastAct >= 10000)) {
    leave_set(false);
  }

  char s[24];
  if (setting) {
    show_hm(editH, editM);
    return;
  }
  if (haveRtc) {
    RTC_DateTime t = rtc.getDateTime();
    int h = t.getHour(), m = t.getMinute();
    if (h != lastShownH || m != lastShownM) {
      lastShownH = h;
      lastShownM = m;
      show_hm(h, m);
    }
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