#include "VanGadget.h"
#include <BLEAdvertisedDevice.h>
#include <string.h>

#define MAGIC 0xA5

struct __attribute__((packed)) SensePkt {
  uint8_t  magic;
  uint8_t  ver;
  int16_t  t1x10;
  int16_t  t2x10;
  uint16_t rpm;
  uint16_t kpa_x10;
};

static float t1 = 0, t2 = 0, kpa = 0;
static uint16_t rpm = 0;
static bool linked = false;
static uint32_t lastRx = 0;

bool  sense_linked() { return linked && (millis() - lastRx < 4000); }
float sense_temp1()  { return t1; }
float sense_temp2()  { return t2; }
float sense_revs()   { return (float)rpm; }
float sense_press()  { return kpa; }

static bool take_pkt(const uint8_t *p) {
  SensePkt pkt;
  memcpy(&pkt, p, sizeof(pkt));
  if (pkt.magic != MAGIC) return false;
  t1 = pkt.t1x10 / 10.0f;
  t2 = pkt.t2x10 / 10.0f;
  rpm = pkt.rpm;
  kpa = pkt.kpa_x10 / 10.0f;
  lastRx = millis();
  linked = true;
  USBSerial.printf("VS t1=%.1f t2=%.1f rpm=%u kPa=%.1f\n", t1, t2, rpm, kpa);
  return true;
}

void sense_on_advert(BLEAdvertisedDevice *d) {
  if (!d) return;
  if (d->haveName()) {
    USBSerial.printf("adv name=%s\n", d->getName().c_str());
  }
  if (!d->haveManufacturerData()) return;
  String md = d->getManufacturerData();
  const uint8_t *b = (const uint8_t *)md.c_str();
  size_t n = (size_t)md.length();
  USBSerial.printf("mfg n=%u ", (unsigned)n);
  for (size_t i = 0; i < n && i < 16; i++) USBSerial.printf("%02X ", b[i]);
  USBSerial.println();
  if (n < sizeof(SensePkt)) return;
  for (size_t i = 0; i + sizeof(SensePkt) <= n; i++) {
    if (b[i] == MAGIC && take_pkt(b + i)) return;
  }
}

void sense_tick() {
  if (linked && millis() - lastRx > 4000) linked = false;
}