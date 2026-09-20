#include "VanGadget.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

#ifndef MINTY_NAME
#define MINTY_NAME "Minty Battery"
#endif
#ifndef MINTY_MAC
#define MINTY_MAC  "a5:c2:39:26:e5:93"
#endif

static BLEUUID serviceUUID((uint16_t)0xFF00);
static BLEUUID rxUUID((uint16_t)0xFF01);
static BLEUUID txUUID((uint16_t)0xFF02);

static bool doConnect = false;
static bool connected = false;
static BLEAdvertisedDevice *myDevice = nullptr;
static BLERemoteCharacteristic *pTxChar = nullptr;
static BLEClient *pClient = nullptr;
static uint8_t bmsBuffer[64];
static size_t bufferIdx = 0;
static uint32_t lastReq = 0;

static float leisureVolt = 0;
static float leisureAmp = 0;
static int   leisureSoc = 0;
static bool  leisureLinked = false;
static char  leisureMsg[32] = "searching Minty";

bool        leisure_linked() { return leisureLinked; }
int         leisure_soc()    { return leisureSoc; }
float       leisure_volt()   { return leisureVolt; }
float       leisure_amp()    { return leisureAmp; }
const char *leisure_msg()    { return leisureMsg; }

static bool nameOrMacMatch(BLEAdvertisedDevice *d) {
  if (!d) return false;
  String mac = d->getAddress().toString();
  mac.toLowerCase();
  String want = String(MINTY_MAC);
  want.toLowerCase();
  if (mac == want) return true;
  if (d->haveName() && d->getName() == MINTY_NAME) return true;
  return false;
}

static void parseFrame(uint8_t *frame) {
  uint16_t rawV = (frame[4] << 8) | frame[5];
  int16_t rawI = (int16_t)((frame[6] << 8) | frame[7]);
  leisureVolt = rawV / 100.0f;
  leisureAmp = rawI / 100.0f;
  leisureSoc = frame[23];
  if (leisureSoc > 100) leisureSoc = 100;
  if (leisureSoc < 0) leisureSoc = 0;
  leisureLinked = true;
  snprintf(leisureMsg, sizeof(leisureMsg), "Minty live");
}

static void notifyCallback(BLERemoteCharacteristic *, uint8_t *pData, size_t length, bool) {
  if (bufferIdx + length >= sizeof(bmsBuffer)) bufferIdx = 0;
  memcpy(&bmsBuffer[bufferIdx], pData, length);
  bufferIdx += length;
  if (bufferIdx < 30) return;
  int startPos = -1;
  for (size_t i = 0; i + 4 < bufferIdx; i++) {
    if (bmsBuffer[i] == 0xDD && bmsBuffer[i + 1] == 0x03) {
      startPos = (int)i;
      break;
    }
  }
  if (startPos >= 0 && (bufferIdx - (size_t)startPos) >= 24) {
    parseFrame(&bmsBuffer[startPos]);
    bufferIdx = 0;
  }
}

class MyClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient *) { connected = true; }
  void onDisconnect(BLEClient *) {
    connected = false;
    leisureLinked = false;
    pTxChar = nullptr;
    snprintf(leisureMsg, sizeof(leisureMsg), "link dropped");
    BLEDevice::getScan()->start(0, nullptr, false);
  }
};

class AdvertisedCB : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    sense_on_advert(&advertisedDevice);
    if (!nameOrMacMatch(&advertisedDevice)) return;
    BLEDevice::getScan()->stop();
    if (myDevice) delete myDevice;
    myDevice = new BLEAdvertisedDevice(advertisedDevice);
    doConnect = true;
  }
};

static bool connectToServer() {
  snprintf(leisureMsg, sizeof(leisureMsg), "connecting");
  if (!pClient) {
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallbacks());
  }
  if (!pClient->connect(myDevice)) return false;

  BLERemoteService *svc = pClient->getService(serviceUUID);
  if (!svc) { pClient->disconnect(); return false; }
  pTxChar = svc->getCharacteristic(txUUID);
  BLERemoteCharacteristic *rx = svc->getCharacteristic(rxUUID);
  if (!pTxChar || !rx) { pClient->disconnect(); return false; }
  if (rx->canNotify()) rx->registerForNotify(notifyCallback);
  connected = true;
  BLEDevice::getScan()->start(0, nullptr, false);
  return true;
}

void ble_begin() {
  
  BLEDevice::init("VanGadget");
  BLEScan *scan = BLEDevice::getScan();
   scan->setAdvertisedDeviceCallbacks(new AdvertisedCB(), true);
  scan->setInterval(160);
  scan->setWindow(160);
  scan->setActiveScan(true);
  scan->start(0, nullptr, false);
}

void ble_tick() {
  if (doConnect) {
    doConnect = false;
    if (!connectToServer()) {
      snprintf(leisureMsg, sizeof(leisureMsg), "retry scan");
      BLEDevice::getScan()->start(0, nullptr, false);
    }
  }
  if (millis() - lastReq < 2000) return;
  lastReq = millis();
  if (connected && pTxChar) {
    uint8_t cmd[] = { 0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77 };
    pTxChar->writeValue(cmd, sizeof(cmd), false);
  }
}