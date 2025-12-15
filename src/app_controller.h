#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "defines.h"
#include <BleController.h>
#include <lilka.h>

namespace ble_controller_app {

// Keyboard layout constants
#define KB_ROWS 5
#define KB_COLS 10
#define KB_LAYERS 3 // lowercase, uppercase, symbols

class AppController {
public:
  AppController();

  void begin();
  void update(const lilka::State &state);

  DeviceMode getMode() const { return currentMode; }
  bool isConnected() const;
  bool isModeSwitchInProgress() const { return modeSwitchInProgress; }
  int getModeSwitchProgress() const;
  void setBatteryLevel(uint8_t level);

  // Keyboard state getters for UI
  int getKeyboardCursorX() const { return kbCursorX; }
  int getKeyboardCursorY() const { return kbCursorY; }
  int getKeyboardLayer() const { return kbLayer; }
  const char *getKeyboardText() const { return kbText; }

private:
  BleController *bleController;
  DeviceMode currentMode;

  // Mode switching
  bool modeSwitchInProgress;
  unsigned long modeSwitchStartTime;
  bool lastSelectStartPressed;

  // Previous button states for edge detection
  bool prevUp, prevDown, prevLeft, prevRight;
  bool prevA, prevB, prevC, prevD;
  bool prevStart, prevSelect;

  // Mouse timing
  unsigned long lastMouseUpdate;

  // Keyboard typing state
  int kbCursorX, kbCursorY;
  int kbLayer;      // 0=lower, 1=upper, 2=symbols
  char kbText[256]; // Buffer for typed text
  int kbTextLen;
  unsigned long lastKeyRepeat;

  void checkModeSwitch(const lilka::State &state);
  void switchToNextMode();

  void handleGamepadMode(const lilka::State &state);
  void handleMouseMode(const lilka::State &state);
  void handleKeyboardMode(const lilka::State &state);

  void updateAxes(const lilka::State &state);
  void typeSelectedKey();
  void sendKeyboardText();
};

} // namespace ble_controller_app

#endif // APP_CONTROLLER_H
