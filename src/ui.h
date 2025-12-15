#pragma once

#include "defines.h"
#include <Arduino.h>
#include <lilka.h>

namespace ble_controller_app {

class UI {
public:
  UI(uint16_t width, uint16_t height);
  void drawFrame(DeviceMode mode, bool isConnected, int batteryLevel,
                 bool modeSwitchInProgress, int switchProgress,
                 const lilka::State &state, int kbCursorX = 0,
                 int kbCursorY = 0, int kbLayer = 0,
                 const char *kbText = nullptr);
  lilka::Canvas *getFrameBuffer();

private:
  lilka::Canvas buffer;
  const uint16_t width, height;

  void drawHeader(DeviceMode mode, bool isConnected, int batteryLevel);
  void drawModeIndicator(DeviceMode mode);
  void drawConnectionStatus(bool isConnected);
  void drawBatteryLevel(int batteryLevel);
  void drawModeSwitchProgress(int progress);
  void drawGamepadMode(const lilka::State &state);
  void drawMouseMode(const lilka::State &state);
  void drawKeyboardMode(const lilka::State &state, int cursorX, int cursorY,
                        int layer, const char *text);
  void drawPressedKeys(const lilka::State &state);
  void drawInstructions(DeviceMode mode);

  const char *getModeString(DeviceMode mode);
};

} // namespace ble_controller_app
