#include "VanGadget.h"
#include "ESP_I2S.h"
#include "es8311.h"

#define SR 16000
#define TONE_HZ 440.0f

static I2SClass i2s;
static es8311_handle_t codec = NULL;
static int16_t wav[256];
static uint32_t ph, phStep;
static const int volMap[4] = { 0, 35, 70, 95 };

static void apply_vol() {
  if (!codec) return;
  int v = (toneOn && app == APP_LEVEL) ? volMap[toneLevel] : 0;
  es8311_voice_volume_set(codec, v, NULL);
  digitalWrite(PA, v > 0 ? HIGH : LOW);
}

void audio_set_level(int level) {
  if (level < 1) level = 1;
  if (level > 3) level = 3;
  toneLevel = level;
  apply_vol();
}

void audio_set_enabled(bool on) {
  toneOn = on;
  apply_vol();
}

static void toneTask(void *) {
  bool pip = false;
  uint32_t tPip = 0;
  int16_t frame[256];
  for (;;) {
    const bool live = audioOk && (app == APP_LEVEL) && toneOn;
    float tilt = tilt_deg();
    bool sound = false;
    if (live) {
      if (tilt <= 0.8f) sound = true;
      else {
        float t = vg_clamp((tilt - 0.8f) / (20.0f - 0.8f), 0, 1);
        uint32_t period = (uint32_t)(90 + t * 700);
        if (millis() - tPip >= (pip ? 50 : period - 50)) {
          pip = !pip;
          tPip = millis();
        }
        sound = pip;
      }
    } else {
      pip = false;
    }

    float near = 1.0f - vg_clamp(tilt / 20.0f, 0, 1);
    int a2 = (int)(9 * (1 - 0.5f * near)), a3 = (int)(5 * (1 - near));
    for (int i = 0; i < 128; i++) {
      int16_t s = 0;
      if (sound) {
        int mix = (wav[(ph >> 16) & 255] * 16
                   + wav[((ph * 2) >> 16) & 255] * a2
                   + wav[((ph * 3) >> 16) & 255] * a3) / 16;
        s = (int16_t)constrain(mix, -28000, 28000);
        ph += phStep;
      }
      frame[i * 2] = frame[i * 2 + 1] = s;
    }
    i2s.write((uint8_t *)frame, sizeof(frame));
  }
}

void audio_begin() {
  pinMode(PA, OUTPUT);
  digitalWrite(PA, HIGH);
  i2s.setPins(9, 45, 8, 10, 16);
  if (!i2s.begin(I2S_MODE_STD, SR, I2S_DATA_BIT_WIDTH_16BIT,
                 I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
    USBSerial.println("I2S fail");
    return;
  }
  codec = es8311_create(0, ES8311_ADDRRES_0);
  if (!codec) {
    USBSerial.println("ES8311 missing");
    return;
  }
  es8311_clock_config_t clk = { false, false, true, SR * 256, SR };
  if (es8311_init(codec, &clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16) != ESP_OK) {
    USBSerial.println("ES8311 init fail");
    return;
  }
  es8311_sample_frequency_config(codec, clk.mclk_frequency, clk.sample_frequency);
  es8311_microphone_config(codec, false);
  apply_vol();
  for (int i = 0; i < 256; i++) {
    wav[i] = (int16_t)(sinf(i * 6.2831853f / 256.0f) * 14000);
  }
  phStep = (uint32_t)((TONE_HZ * 256.0f * 65536.0f) / SR);
  audioOk = true;
  xTaskCreatePinnedToCore(toneTask, "tone", 4096, NULL, 1, NULL, 0);
}
