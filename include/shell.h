#pragma once
#include <lvgl.h>
#include "app_id.h"

extern lv_obj_t *statusBar, *sbTime, *sbBatt, *sbBle, *homeBar;
extern lv_obj_t *home;
extern lv_obj_t *page[APP_COUNT];
extern lv_obj_t *dial, *bubble;
extern lv_obj_t *battPct, *battVolt, *battState;
extern lv_obj_t *clkTime, *clkDate, *clkTemp;
extern lv_obj_t *engTemp, *engHint;
extern lv_obj_t *sysBody;

extern volatile int app;
extern uint32_t tUi;

float vg_clamp(float v, float a, float b);
lv_obj_t *vg_page();
lv_obj_t *vg_txt(lv_obj_t *p, const char *t, const lv_font_t *f, lv_coord_t y);
lv_obj_t *vg_icon(lv_obj_t *parent, const char *title, lv_color_t col, int appId, int colx, int row);
void show_app(int id);

void home_build();
void more_build();
void soon_build();
void level_build();
void level_tick();
void batt_build();
void batt_tick();
void clock_build();
void clock_tick();
bool clock_setting();
void clock_on_flick();
void eng_build();
void eng_tick();
void torch_build();
void torch_tick();
void sys_build();
void sys_tick();
void shell_build();
void shell_tick();