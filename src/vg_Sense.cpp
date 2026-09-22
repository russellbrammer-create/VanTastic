#include "VanGadget.h"
#include <BLEAdvertisedDevice.h>
#include <string.h>

#define MAGIC 0xA5
#ifndef VG_SENSE_TRACE
#define VG_SENSE_TRACE 0
#endif

#define SENSE_F_FROST   0x01
#define SENSE_F_OVERREV 0x02
#define SENSE_F_DUMMY   0x80

struct __attribute__((packed)) SensePktV1 {
  uint8_t  magic;
  uint8_t  ver;
  int16_t  t1x10;
  int16_t  t2x10;
  uint16_t rpm;
  uint16_t kpa_x10;
};

struct __attribute__((packed)) SensePkt {
  uint8_t  magic;
  uint8_t  ver;
  int16_t  t_block_x10;
  int16_t  t_cool_x10;
  int16_t  t_out_x10;
  uint16_t rpm;
  uint16_t kpa_x10;
  uint16_t vbat_x100;
  uint8_t  flags;
};

static float tBlock = 0, tCool = 0, tOut = 0, kpa = 0, vbat = 0;
static uint16_t rpm = 0;
static uint8_t flags = 0;
static bool linked = false;
static uint32_t lastRx = 0;

bool  sense_linked()   { return linked && (millis() - lastRx < 4000); }
float sense_temp1()    { return tBlock; }
float sense_temp2()    { return tCool; }
float sense_temp_out() { return tOut; }
float sense_revs()     { return (float)rpm; }
float sense_press()    { return kpa; }
float sense_vbat()     { return vbat; }
bool  sense_frost()    { return sense_linked() && (flags & SENSE_F_FROST); }
bool  sense_overrev()  { return sense_linked() && (flags & SENSE_F_OVERREV); }
bool  sense_dummy()    { return sense_linked() && (flags & SENSE_F_DUMMY); }

static bool take_pkt(const uint8_t *p, size_t n) {
  if (n < sizeof(SensePktV1) || p[0] != MAGIC) return false;
  uint8_t ver = p[1];
  if (ver >= 2 && n >= sizeof(SensePkt)) {
    SensePkt pkt;
    memcpy(&pkt, p, sizeof(pkt));
    tBlock = pkt.t_block_x10 / 10.0f;
    tCool  = pkt.t_cool_x10 / 10.0f;
    tOut   = pkt.t_out_x10 / 10.0f;
    rpm    = pkt.rpm;
    kpa    = pkt.kpa_x10 / 10.0f;
    vbat   = pkt.vbat_x100 / 100.0f;
    flags  = pkt.flags;
  } else {
    SensePktV1 pkt;
    memcpy(&pkt, p, sizeof(pkt));
    tBlock = pkt.t1x10 / 10.0f;
    tCool  = pkt.t2x10 / 10.0f;
    tOut   = 0;
    rpm    = pkt.rpm;
    kpa    = pkt.kpa_x10 / 10.0f;
    vbat   = 0;
    flags  = 0;
  }
  lastRx = millis();
  linked = true;
#if VG_SENSE_TRACE
  USBSerial.printf("VS blk=%.1f cool=%.1f out=%.1f rpm=%u V=%.2f f=%02X\n",
                   tBlock, tCool, tOut, rpm, vbat, flags);
#endif
  return true;
}

void sense_on_advert(BLEAdvertisedDevice *d) {
  if (!d) return;
#if VG_SENSE_TRACE
  if (d->haveName()) {
    USBSerial.printf("adv name=%s\n", d->getName().c_str());
  }
#endif
  if (!d->haveManufacturerData()) return;
  String md = d->getManufacturerData();
  const uint8_t *b = (const uint8_t *)md.c_str();
  size_t n = (size_t)md.length();
#if VG_SENSE_TRACE
  USBSerial.printf("mfg n=%u ", (unsigned)n);
  for (size_t i = 0; i < n && i < 20; i++) USBSerial.printf("%02X ", b[i]);
  USBSerial.println();
#endif
  if (n < sizeof(SensePktV1)) return;
  for (size_t i = 0; i + sizeof(SensePktV1) <= n; i++) {
    if (b[i] == MAGIC && take_pkt(b + i, n - i)) return;
  }
}

void sense_tick() {
  if (linked && millis() - lastRx > 4000) linked = false;
}
