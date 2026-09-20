#include "VanGadget.h"

HWCDC USBSerial;
SensorQMI8658 qmi;
SensorPCF85063 rtc;
XPowersAXP2101 pmu;
IMUdata acc;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LCD_WIDTH * 40];

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_CO5300 *gfx = new Arduino_CO5300(
    bus, GFX_NOT_DEFINED, 0, LCD_WIDTH, LCD_HEIGHT, 16, 0, 0, 0);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> TP(new Arduino_CST816x(
    IIC_Bus, CST816T_DEVICE_ADDRESS, DRIVEBUS_DEFAULT_VALUE, TP_INT,
    Arduino_IIC_Touch_Interrupt));
void Arduino_IIC_Touch_Interrupt(void) { TP->IIC_Interrupt_Flag = true; }

lv_obj_t *statusBar, *sbTime, *sbBatt, *sbBle, *homeBar;
lv_obj_t *home;
lv_obj_t *page[APP_COUNT];
lv_obj_t *dial, *bubble;
lv_obj_t *battPct, *battVolt, *battState;
lv_obj_t *clkTime, *clkDate, *clkTemp;
lv_obj_t *engTemp, *engHint;
lv_obj_t *sysBody;

volatile int app = APP_HOME;
bool havePmu, haveRtc, torchOn;
uint32_t tUi;
uint8_t brightness = 180;
bool bleWanted = true;
int toneLevel = 2;
volatile bool toneOn = true;
bool audioOk = false;

int16_t gestureY0 = -1;
bool gestureArmed = false;

#define IDLE_DIM_MS  20000
#define IDLE_DIM_BRI 40

static uint32_t lastTouchMs = 0;
static bool screenDimmed = false;

static void idle_dim_tick() {
  if (app == APP_TORCH) {
    screenDimmed = false;
    return;
  }
  if (screenDimmed) return;
  if (millis() - lastTouchMs < IDLE_DIM_MS) return;
  gfx->setBrightness(IDLE_DIM_BRI);
  screenDimmed = true;
}

static void idle_on_touch() {
  lastTouchMs = millis();
  if (!screenDimmed) return;
  if (app == APP_TORCH) return;
  gfx->setBrightness(brightness);
  screenDimmed = false;
}

void on_flush(lv_disp_drv_t *d, const lv_area_t *a, lv_color_t *c) {
  uint32_t w = a->x2 - a->x1 + 1, h = a->y2 - a->y1 + 1;
#if LV_COLOR_16_SWAP
  gfx->draw16bitBeRGBBitmap(a->x1, a->y1, (uint16_t *)&c->full, w, h);
#else
  gfx->draw16bitRGBBitmap(a->x1, a->y1, (uint16_t *)&c->full, w, h);
#endif
  lv_disp_flush_ready(d);
}

void on_tick(void *) { lv_tick_inc(2); }

void on_touch(lv_indev_drv_t *, lv_indev_data_t *data) {
  int32_t n = TP->IIC_Read_Device_Value(
      TP->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
  int32_t x = TP->IIC_Read_Device_Value(
      TP->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
  int32_t y = TP->IIC_Read_Device_Value(
      TP->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

  if (n > 0 && x >= 0 && y >= 0) {
    idle_on_touch();
    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;
    if (!gestureArmed && y > LCD_HEIGHT - DOCK_ZONE) {
      gestureArmed = true;
      gestureY0 = y;
    }
  } else {
    if (gestureArmed && gestureY0 > 0 && data->point.y > 0 &&
        gestureY0 - data->point.y > 28) {
      if (app == APP_CLOCK && clock_setting()) {
        clock_on_flick();
      } else if (app == APP_HOME) {
        show_app(APP_MORE);
      } else {
        show_app(APP_HOME);
      }
    }
    gestureArmed = false;
    gestureY0 = -1;
    data->state = LV_INDEV_STATE_REL;
  }
}

void show_app(int id) {
  app = id;
  if (id != APP_TORCH && torchOn) {
    torchOn = false;
    if (!screenDimmed) gfx->setBrightness(brightness);
  }
  if (id != APP_TORCH && !screenDimmed) {
    gfx->setBrightness(brightness);
  }
  audio_set_enabled(toneOn);
  if (homeBar) lv_obj_clear_flag(homeBar, LV_OBJ_FLAG_HIDDEN);
  for (int i = 0; i < APP_COUNT; i++) {
    if (!page[i]) continue;
    if (i == id) lv_obj_clear_flag(page[i], LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(page[i], LV_OBJ_FLAG_HIDDEN);
  }
}

void setup() {
  USBSerial.begin(115200);
  Wire.begin(IIC_SDA, IIC_SCL);
  TP->begin();
  gfx->begin();
  gfx->setBrightness(brightness);
  lastTouchMs = millis();
  screenDimmed = false;

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_WIDTH * 40);
  static lv_disp_drv_t dd;
  lv_disp_drv_init(&dd);
  dd.hor_res = LCD_WIDTH;
  dd.ver_res = LCD_HEIGHT;
  dd.flush_cb = on_flush;
  dd.draw_buf = &draw_buf;
  lv_disp_drv_register(&dd);
  static lv_indev_drv_t id;
  lv_indev_drv_init(&id);
  id.type = LV_INDEV_TYPE_POINTER;
  id.read_cb = on_touch;
  lv_indev_drv_register(&id);
  const esp_timer_create_args_t ta = {.callback = &on_tick, .name = "t"};
  esp_timer_handle_t th = NULL;
  esp_timer_create(&ta, &th);
  esp_timer_start_periodic(th, 2000);

  qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                          SensorQMI8658::ACC_ODR_250Hz,
                          SensorQMI8658::LPF_MODE_3);
  qmi.enableAccelerometer();
  haveRtc = rtc.begin(Wire, IIC_SDA, IIC_SCL);
  havePmu = pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (havePmu) {
    pmu.enableBattDetection();
    pmu.enableBattVoltageMeasure();
    pmu.enableVbusVoltageMeasure();
    pmu.enableTemperatureMeasure();
  }

  audio_begin();
  ble_begin();
  shell_build();
  show_app(APP_HOME);
}

void loop() {
  lv_timer_handler();
  switch (app) {
    case APP_BATT:  batt_tick();  break;
    case APP_CLOCK: clock_tick(); break;
    case APP_ENG:   eng_tick();   break;
    case APP_TORCH: torch_tick(); break;
    case APP_SYS:   sys_tick();   break;
    default: break;
  }
  level_tick();
  ble_tick();
  sense_tick();
  shell_tick();
  idle_dim_tick();
  delay(5);
}