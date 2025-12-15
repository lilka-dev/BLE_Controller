#include "app_controller.h"
#include <Arduino.h>
#include <BleKeyboardKeys.h>

#define MOUSE_UPDATE_INTERVAL 20

// HID Key codes (USB HID specification)
#define HID_KEY_A 0x04
#define HID_KEY_Z 0x1D
#define HID_KEY_1 0x1E
#define HID_KEY_0 0x27
#define HID_KEY_ENTER 0x28
#define HID_KEY_BACKSPACE 0x2A
#define HID_KEY_SPACE 0x2C
#define HID_KEY_MINUS 0x2D
#define HID_KEY_EQUAL 0x2E
#define HID_KEY_BRACKET_LEFT 0x2F
#define HID_KEY_BRACKET_RIGHT 0x30
#define HID_KEY_BACKSLASH 0x31
#define HID_KEY_SEMICOLON 0x33
#define HID_KEY_APOSTROPHE 0x34
#define HID_KEY_GRAVE 0x35
#define HID_KEY_COMMA 0x36
#define HID_KEY_PERIOD 0x37
#define HID_KEY_SLASH 0x38
#define HID_KEY_TAB 0x2B
#define HID_KEY_ESCAPE 0x29
#define HID_KEY_DELETE 0x4C
#define HID_KEY_RIGHT_ARROW 0x4F
#define HID_KEY_LEFT_ARROW 0x50
#define HID_KEY_DOWN_ARROW 0x51
#define HID_KEY_UP_ARROW 0x52

#define HID_MOD_LSHIFT 0x02
#define HID_MOD_LEFT_GUI 0x08

// Special key markers
#define SPECIAL_ENTER '\x01'
#define SPECIAL_TAB '\x02'
#define SPECIAL_ESC '\x03'
#define SPECIAL_LEFT '\x04'
#define SPECIAL_RIGHT '\x05'
#define SPECIAL_UP '\x06'
#define SPECIAL_DOWN '\x07'
#define SPECIAL_DEL '\x08'
#define SPECIAL_HOME '\x09'
#define SPECIAL_END '\x0A'

// Keyboard layouts (Row 5: Ent Tab Esc <- -> Up Dn Del Hm End)
static const char *KB_LAYOUT_LOWER[KB_ROWS] = {
    "1234567890", "qwertyuiop", "asdfghjkl;", "zxcvbnm,./",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

static const char *KB_LAYOUT_UPPER[KB_ROWS] = {
    "!@#$%^&*()", "QWERTYUIOP", "ASDFGHJKL:", "ZXCVBNM<>?",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

static const char *KB_LAYOUT_SYMBOLS[KB_ROWS] = {
    "1234567890", "-=[]\\|`~  ", "!@#$%^&*()", "_+{}|~    ",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

namespace ble_controller_app {

AppController::AppController()
    : bleController(nullptr), currentMode(MODE_GAMEPAD),
      modeSwitchInProgress(false), modeSwitchStartTime(0),
      lastSelectStartPressed(false), prevUp(false), prevDown(false),
      prevLeft(false), prevRight(false), prevA(false), prevB(false),
      prevC(false), prevD(false), prevStart(false), prevSelect(false),
      lastMouseUpdate(0), kbCursorX(0), kbCursorY(1), kbLayer(0), kbTextLen(0),
      lastKeyRepeat(0) {
  memset(kbText, 0, sizeof(kbText));
}

void AppController::begin() {
  BleControllerConfiguration *config = new BleControllerConfiguration();
  config->setControllerType(CONTROLLER_TYPE_CONTROLLER);
  config->setAutoReport(false);
  config->setButtonCount(16);
  config->setHatSwitchCount(0);
  config->setWhichAxes(true, true, false, false, false, false, false, false);
  config->setAxesMin(AXIS_MIN);
  config->setAxesMax(AXIS_MAX);
  config->setHidReportId(
      CONTROLLER_REPORT_ID); // Report ID 1 (not 3 - conflicts with mouse)

  bleController = new BleController(DEVICE_NAME);
  bleController->begin(config);
}

bool AppController::isConnected() const {
  return bleController && bleController->isConnected();
}

void AppController::setBatteryLevel(uint8_t level) {
  if (bleController) {
    bleController->setBatteryLevel(level);
  }
}

int AppController::getModeSwitchProgress() const {
  if (!modeSwitchInProgress)
    return 0;

  unsigned long elapsed = millis() - modeSwitchStartTime;
  int progress = (elapsed * 100) / MODE_SWITCH_HOLD_TIME;
  return min(progress, 100);
}

void AppController::checkModeSwitch(const lilka::State &state) {
  bool selectStartPressed = state.select.pressed && state.start.pressed;

  if (selectStartPressed) {
    if (!lastSelectStartPressed) {
      modeSwitchInProgress = true;
      modeSwitchStartTime = millis();
    } else if (modeSwitchInProgress) {
      if (millis() - modeSwitchStartTime >= MODE_SWITCH_HOLD_TIME) {
        switchToNextMode();
        modeSwitchInProgress = false;
      }
    }
  } else {
    modeSwitchInProgress = false;
  }

  lastSelectStartPressed = selectStartPressed;
}

void AppController::switchToNextMode() {
  DeviceMode previousMode = currentMode;

  switch (currentMode) {
  case MODE_GAMEPAD:
    currentMode = MODE_MOUSE;
    Serial.println("Mode: MOUSE");
    break;
  case MODE_MOUSE:
    currentMode = MODE_KEYBOARD;
    Serial.println("Mode: KEYBOARD");
    break;
  case MODE_KEYBOARD:
    currentMode = MODE_GAMEPAD;
    Serial.println("Mode: GAMEPAD");
    break;
  }

  if (bleController) {
    for (int i = 1; i <= 16; i++) {
      bleController->release(i);
    }
    bleController->setX(AXIS_CENTER);
    bleController->setY(AXIS_CENTER);
    bleController->sendReport();
    bleController->setKeyboardModifiers(0);
    bleController->keyboardReleaseAll();
    bleController->mouseReleaseAll();
  }
}

void AppController::update(const lilka::State &state) {
  if (!bleController || !bleController->isConnected()) {
    prevUp = state.up.pressed;
    prevDown = state.down.pressed;
    prevLeft = state.left.pressed;
    prevRight = state.right.pressed;
    prevA = state.a.pressed;
    prevB = state.b.pressed;
    prevC = state.c.pressed;
    prevD = state.d.pressed;
    prevStart = state.start.pressed;
    prevSelect = state.select.pressed;
    checkModeSwitch(state);
    return;
  }

  checkModeSwitch(state);

  if (modeSwitchInProgress) {
    return;
  }

  switch (currentMode) {
  case MODE_GAMEPAD:
    handleGamepadMode(state);
    break;
  case MODE_MOUSE:
    bleController->setX(AXIS_CENTER);
    bleController->setY(AXIS_CENTER);
    handleMouseMode(state);
    break;
  case MODE_KEYBOARD:
    bleController->setX(AXIS_CENTER);
    bleController->setY(AXIS_CENTER);
    handleKeyboardMode(state);
    break;
  }

  prevUp = state.up.pressed;
  prevDown = state.down.pressed;
  prevLeft = state.left.pressed;
  prevRight = state.right.pressed;
  prevA = state.a.pressed;
  prevB = state.b.pressed;
  prevC = state.c.pressed;
  prevD = state.d.pressed;
  prevStart = state.start.pressed;
  prevSelect = state.select.pressed;
}

void AppController::updateAxes(const lilka::State &state) {
  int16_t x = AXIS_CENTER;
  int16_t y = AXIS_CENTER;

  if (state.left.pressed)
    x = AXIS_MIN;
  else if (state.right.pressed)
    x = AXIS_MAX;

  if (state.up.pressed)
    y = AXIS_MIN;
  else if (state.down.pressed)
    y = AXIS_MAX;

  bleController->setX(x);
  bleController->setY(y);
}

void AppController::handleGamepadMode(const lilka::State &state) {
  bool needReport = false;

  updateAxes(state);

  if (state.up.pressed != prevUp || state.down.pressed != prevDown ||
      state.left.pressed != prevLeft || state.right.pressed != prevRight) {
    needReport = true;
  }

  if (state.a.pressed != prevA) {
    if (state.a.pressed)
      bleController->press(BUTTON_1);
    else
      bleController->release(BUTTON_1);
    needReport = true;
  }

  if (state.b.pressed != prevB) {
    if (state.b.pressed)
      bleController->press(BUTTON_2);
    else
      bleController->release(BUTTON_2);
    needReport = true;
  }

  if (state.c.pressed != prevC) {
    if (state.c.pressed)
      bleController->press(BUTTON_3);
    else
      bleController->release(BUTTON_3);
    needReport = true;
  }

  if (state.d.pressed != prevD) {
    if (state.d.pressed)
      bleController->press(BUTTON_4);
    else
      bleController->release(BUTTON_4);
    needReport = true;
  }

  if (state.start.pressed != prevStart) {
    if (state.start.pressed)
      bleController->press(BUTTON_5);
    else
      bleController->release(BUTTON_5);
    needReport = true;
  }

  if (state.select.pressed != prevSelect) {
    if (state.select.pressed)
      bleController->press(BUTTON_6);
    else
      bleController->release(BUTTON_6);
    needReport = true;
  }

  if (needReport) {
    bleController->sendReport();
  }
}

void AppController::handleMouseMode(const lilka::State &state) {
  int8_t moveX = 0;
  int8_t moveY = 0;

  if (state.left.pressed)
    moveX = -MOUSE_SPEED;
  else if (state.right.pressed)
    moveX = MOUSE_SPEED;

  if (state.up.pressed)
    moveY = -MOUSE_SPEED;
  else if (state.down.pressed)
    moveY = MOUSE_SPEED;

  if (moveX != 0 || moveY != 0) {
    bleController->mouseMove(moveX, moveY);
  }

  if (state.a.pressed && !prevA) {
    bleController->mouseClick(MOUSE_LEFT);
  }

  if (state.b.pressed && !prevB) {
    bleController->mouseClick(MOUSE_RIGHT);
  }

  if (state.c.pressed) {
    bleController->mouseScroll(-SCROLL_SPEED);
  }

  if (state.d.pressed) {
    bleController->mouseScroll(SCROLL_SPEED);
  }

  if (state.start.pressed && !prevStart) {
    bleController->mouseClick(MOUSE_MIDDLE);
  }
}

void AppController::handleKeyboardMode(const lilka::State &state) {
  if (state.up.pressed && !prevUp) {
    kbCursorY = (kbCursorY - 1 + KB_ROWS) % KB_ROWS;
  }
  if (state.down.pressed && !prevDown) {
    kbCursorY = (kbCursorY + 1) % KB_ROWS;
  }
  if (state.left.pressed && !prevLeft) {
    kbCursorX = (kbCursorX - 1 + KB_COLS) % KB_COLS;
  }
  if (state.right.pressed && !prevRight) {
    kbCursorX = (kbCursorX + 1) % KB_COLS;
  }

  if (state.a.pressed && !prevA) {
    typeSelectedKey();
  }

  if (state.b.pressed && !prevB) {
    if (kbTextLen > 0) {
      kbTextLen--;
      kbText[kbTextLen] = '\0';
    }
    bleController->sendRawKeyboard(0, HID_KEY_BACKSPACE);
    delay(15);
    bleController->sendRawKeyboard(0, 0);
  }

  if (state.c.pressed && !prevC) {
    kbLayer = (kbLayer + 1) % KB_LAYERS;
  }

  if (state.d.pressed && !prevD) {
    if (kbTextLen < 255) {
      kbText[kbTextLen++] = ' ';
      kbText[kbTextLen] = '\0';
    }
    bleController->sendRawKeyboard(0, HID_KEY_SPACE);
    delay(15);
    bleController->sendRawKeyboard(0, 0);
  }

  if (state.start.pressed && !prevStart) {
    bleController->sendRawKeyboard(0, HID_KEY_ENTER);
    delay(15);
    bleController->sendRawKeyboard(0, 0);
  }

  if (state.select.pressed && !prevSelect) {
    bleController->sendRawKeyboard(HID_MOD_LEFT_GUI, HID_KEY_SPACE);
    delay(15);
    bleController->sendRawKeyboard(0, 0);
  }
}

void AppController::typeSelectedKey() {
  const char *layout;
  switch (kbLayer) {
  case 0:
    layout = KB_LAYOUT_LOWER[kbCursorY];
    break;
  case 1:
    layout = KB_LAYOUT_UPPER[kbCursorY];
    break;
  case 2:
    layout = KB_LAYOUT_SYMBOLS[kbCursorY];
    break;
  default:
    layout = KB_LAYOUT_LOWER[kbCursorY];
    break;
  }

  char key = layout[kbCursorX];
  if (key == '\0' || key == ' ')
    return;

  uint8_t hidCode = 0;
  switch (key) {
  case SPECIAL_ENTER:
    hidCode = HID_KEY_ENTER;
    break;
  case SPECIAL_TAB:
    hidCode = HID_KEY_TAB;
    break;
  case SPECIAL_ESC:
    hidCode = HID_KEY_ESCAPE;
    break;
  case SPECIAL_LEFT:
    hidCode = HID_KEY_LEFT_ARROW;
    break;
  case SPECIAL_RIGHT:
    hidCode = HID_KEY_RIGHT_ARROW;
    break;
  case SPECIAL_UP:
    hidCode = HID_KEY_UP_ARROW;
    break;
  case SPECIAL_DOWN:
    hidCode = HID_KEY_DOWN_ARROW;
    break;
  case SPECIAL_DEL:
    hidCode = HID_KEY_DELETE;
    break;
  case SPECIAL_HOME:
    hidCode = 0x4A;
    break;
  case SPECIAL_END:
    hidCode = 0x4D;
    break;
  default:
    if (kbTextLen < 255) {
      kbText[kbTextLen++] = key;
      kbText[kbTextLen] = '\0';
    }
    bleController->typeChar(key);
    return;
  }

  if (hidCode != 0) {
    bleController->sendRawKeyboard(0, hidCode);
    delay(15);
    bleController->sendRawKeyboard(0, 0);
  }
}

void AppController::sendKeyboardText() {
  if (kbTextLen > 0) {
    bleController->keyboardPrint(kbText);
    kbTextLen = 0;
    kbText[0] = '\0';
  }
}

} // namespace ble_controller_app
