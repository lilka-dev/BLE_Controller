/*
 * Lilka BLE Controller App
 * Multi-mode BLE HID device: Gamepad, Mouse, Keyboard
 * Hold SELECT + START for 3 seconds to switch modes
 */

#include "app_controller.h"
#include "defines.h"
#include "ui.h"
#include <Arduino.h>
#include <lilka.h>

using namespace ble_controller_app;

AppController controller;
UI ui(280, 240);

unsigned long lastBatteryUpdate = 0;
int cachedBatteryLevel = 100;
#define BATTERY_UPDATE_INTERVAL 30000

int getBatteryLevel() {
  unsigned long now = millis();
  if (now - lastBatteryUpdate >= BATTERY_UPDATE_INTERVAL ||
      lastBatteryUpdate == 0) {
    int level = lilka::battery.readLevel();
    if (level >= 0) {
      cachedBatteryLevel = level;
      controller.setBatteryLevel(level);
    }
    lastBatteryUpdate = now;
  }
  return cachedBatteryLevel;
}

void setup() {
  Serial.begin(115200);
  lilka::begin();
  controller.begin();
}

void loop() {
  lilka::State state = lilka::controller.getState();
  controller.update(state);

  ui.drawFrame(controller.getMode(), controller.isConnected(),
               getBatteryLevel(), controller.isModeSwitchInProgress(),
               controller.getModeSwitchProgress(), state,
               controller.getKeyboardCursorX(), controller.getKeyboardCursorY(),
               controller.getKeyboardLayer(), controller.getKeyboardText());

  lilka::display.drawCanvas(ui.getFrameBuffer());
  delay(16);
}
