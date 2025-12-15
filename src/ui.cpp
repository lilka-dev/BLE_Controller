#include "ui.h"
#include "lang/lang.h"
#include <Arduino.h>

// RGB565 colors
#define COLOR_GRAY 0x8410
#define COLOR_NAVY 0x0010
#define COLOR_DARK_GRAY 0x4208
#define COLOR_LIGHT_GRAY 0xC618

namespace ble_controller_app {

UI::UI(uint16_t width, uint16_t height)
    : buffer(width, height), width(width), height(height) {}

lilka::Canvas *UI::getFrameBuffer() { return &buffer; }

const char *UI::getModeString(DeviceMode mode) {
  switch (mode) {
  case MODE_GAMEPAD:
    return STR_MODE_GAMEPAD;
  case MODE_MOUSE:
    return STR_MODE_MOUSE;
  case MODE_KEYBOARD:
    return STR_MODE_KEYBOARD;
  default:
    return STR_MODE_UNKNOWN;
  }
}

void UI::drawFrame(DeviceMode mode, bool isConnected, int batteryLevel,
                   bool modeSwitchInProgress, int switchProgress,
                   const lilka::State &state, int kbCursorX, int kbCursorY,
                   int kbLayer, const char *kbText) {
  buffer.fillScreen(lilka::colors::Black);

  drawHeader(mode, isConnected, batteryLevel);

  switch (mode) {
  case MODE_GAMEPAD:
    drawGamepadMode(state);
    break;
  case MODE_MOUSE:
    drawMouseMode(state);
    break;
  case MODE_KEYBOARD:
    drawKeyboardMode(state, kbCursorX, kbCursorY, kbLayer, kbText);
    break;
  }

  drawPressedKeys(state);
  drawInstructions(mode);

  if (modeSwitchInProgress) {
    drawModeSwitchProgress(switchProgress);
  }
}

void UI::drawHeader(DeviceMode mode, bool isConnected, int batteryLevel) {
  buffer.setFont(FONT_5x7);
  buffer.setTextColor(lilka::colors::White);
  buffer.setCursor(15, 14);
  buffer.print(getModeString(mode));

  drawConnectionStatus(isConnected);
  drawBatteryLevel(batteryLevel);

  buffer.drawLine(10, 20, width - 10, 20, COLOR_GRAY);
}

void UI::drawConnectionStatus(bool isConnected) {
  int centerX = width / 2;
  buffer.setFont(FONT_5x7);

  if (isConnected) {
    buffer.setTextColor(lilka::colors::Green);
    buffer.setCursor(centerX - 25, 12);
    buffer.print(STR_CONNECTED);
  } else {
    buffer.setTextColor(lilka::colors::Red);
    buffer.setCursor(centerX - 30, 12);
    buffer.print(STR_DISCONNECTED);
  }
}

void UI::drawBatteryLevel(int batteryLevel) {
  int battX = width - 50;
  buffer.setFont(FONT_5x7);

  buffer.drawRect(battX, 6, 26, 10, lilka::colors::White);
  buffer.fillRect(battX + 26, 8, 2, 6, lilka::colors::White);

  uint16_t fillColor;
  if (batteryLevel > 50) {
    fillColor = lilka::colors::Green;
  } else if (batteryLevel > 20) {
    fillColor = lilka::colors::Yellow;
  } else {
    fillColor = lilka::colors::Red;
  }

  int fillWidth = (22 * batteryLevel) / 100;
  if (fillWidth > 0) {
    buffer.fillRect(battX + 2, 8, fillWidth, 6, fillColor);
  }
}

void UI::drawModeSwitchProgress(int progress) {
  int centerY = height / 2;

  buffer.fillRect(20, centerY - 30, width - 40, 60, COLOR_NAVY);
  buffer.drawRect(20, centerY - 30, width - 40, 60, lilka::colors::White);

  buffer.setFont(FONT_6x13);
  buffer.setTextColor(lilka::colors::Yellow);
  buffer.setCursor(35, centerY - 10);
  buffer.print(STR_SWITCHING_MODE);

  int barWidth = width - 60;
  int barX = 30;
  int barY = centerY + 5;

  buffer.drawRect(barX, barY, barWidth, 15, lilka::colors::White);

  int fillWidth = (barWidth - 4) * progress / 100;
  buffer.fillRect(barX + 2, barY + 2, fillWidth, 11, lilka::colors::Cyan);

  buffer.setFont(FONT_5x7);
  buffer.setTextColor(lilka::colors::White);
  buffer.setCursor(barX + barWidth / 2 - 10, barY + 25);
  char progressStr[8];
  sprintf(progressStr, "%d%%", progress);
  buffer.print(progressStr);
}

void UI::drawGamepadMode(const lilka::State &state) {
  int centerX = width / 2;
  int centerY = height / 2;

  buffer.setFont(FONT_6x13);
  buffer.setTextColor(lilka::colors::Cyan);
  buffer.setCursor(centerX - 50, 38);
  buffer.print(STR_GAMEPAD_MODE);

  // D-pad
  int dpadX = 60;
  int dpadY = centerY + 10;
  int dpadSize = 14;

  uint16_t upColor = state.up.pressed ? lilka::colors::Green : COLOR_GRAY;
  uint16_t downColor = state.down.pressed ? lilka::colors::Green : COLOR_GRAY;
  uint16_t leftColor = state.left.pressed ? lilka::colors::Green : COLOR_GRAY;
  uint16_t rightColor = state.right.pressed ? lilka::colors::Green : COLOR_GRAY;

  buffer.fillRect(dpadX, dpadY - dpadSize * 2, dpadSize, dpadSize, upColor);
  buffer.fillRect(dpadX, dpadY + dpadSize, dpadSize, dpadSize, downColor);
  buffer.fillRect(dpadX - dpadSize - 5, dpadY - dpadSize / 2, dpadSize,
                  dpadSize, leftColor);
  buffer.fillRect(dpadX + dpadSize + 5, dpadY - dpadSize / 2, dpadSize,
                  dpadSize, rightColor);
  buffer.fillRect(dpadX, dpadY - dpadSize / 2, dpadSize, dpadSize,
                  COLOR_DARK_GRAY);

  // Buttons: diamond layout (C=top, D=left, A=right, B=bottom)
  int btnX = width - 90;
  int btnY = centerY + 10;
  int btnR = 11;

  uint16_t aColor = state.a.pressed ? lilka::colors::Green : COLOR_GRAY;
  uint16_t bColor = state.b.pressed ? lilka::colors::Red : COLOR_GRAY;
  uint16_t cColor = state.c.pressed ? lilka::colors::Blue : COLOR_GRAY;
  uint16_t dColor = state.d.pressed ? lilka::colors::Yellow : COLOR_GRAY;

  buffer.fillCircle(btnX + 25, btnY - 20, btnR, cColor);
  buffer.fillCircle(btnX, btnY, btnR, dColor);
  buffer.fillCircle(btnX + 50, btnY, btnR, aColor);
  buffer.fillCircle(btnX + 25, btnY + 20, btnR, bColor);

  buffer.setFont(FONT_5x7);
  buffer.setTextColor(lilka::colors::Black);
  buffer.setCursor(btnX + 22, btnY - 17);
  buffer.print(STR_KEY_C);
  buffer.setCursor(btnX - 3, btnY + 3);
  buffer.print(STR_KEY_D);
  buffer.setCursor(btnX + 47, btnY + 3);
  buffer.print(STR_KEY_A);
  buffer.setCursor(btnX + 22, btnY + 23);
  buffer.print(STR_KEY_B);
}

void UI::drawMouseMode(const lilka::State &state) {
  int centerX = width / 2;
  int centerY = height / 2 - 5;

  buffer.setFont(FONT_6x13);
  buffer.setTextColor(lilka::colors::Magenta);
  buffer.setCursor(centerX - 45, 38);
  buffer.print(STR_MOUSE_MODE);

  // Mouse icon
  buffer.drawRect(centerX - 18, centerY - 15, 36, 50, lilka::colors::White);
  buffer.drawLine(centerX, centerY - 15, centerX, centerY + 10, COLOR_GRAY);
  buffer.drawLine(centerX - 18, centerY + 10, centerX + 18, centerY + 10,
                  COLOR_GRAY);

  uint16_t leftColor = state.a.pressed ? lilka::colors::Green : COLOR_GRAY;
  buffer.fillRect(centerX - 16, centerY - 13, 14, 22, leftColor);

  uint16_t rightColor = state.b.pressed ? lilka::colors::Red : COLOR_GRAY;
  buffer.fillRect(centerX + 2, centerY - 13, 14, 22, rightColor);

  // Direction indicator
  int indicatorX = centerX;
  int indicatorY = centerY + 50;
  int arrowLen = 12;

  if (state.up.pressed || state.down.pressed || state.left.pressed ||
      state.right.pressed) {
    if (state.up.pressed) {
      buffer.fillTriangle(indicatorX, indicatorY - arrowLen, indicatorX - 7,
                          indicatorY, indicatorX + 7, indicatorY,
                          lilka::colors::Green);
    }
    if (state.down.pressed) {
      buffer.fillTriangle(indicatorX, indicatorY + arrowLen, indicatorX - 7,
                          indicatorY, indicatorX + 7, indicatorY,
                          lilka::colors::Green);
    }
    if (state.left.pressed) {
      buffer.fillTriangle(indicatorX - arrowLen, indicatorY, indicatorX,
                          indicatorY - 7, indicatorX, indicatorY + 7,
                          lilka::colors::Green);
    }
    if (state.right.pressed) {
      buffer.fillTriangle(indicatorX + arrowLen, indicatorY, indicatorX,
                          indicatorY - 7, indicatorX, indicatorY + 7,
                          lilka::colors::Green);
    }
  } else {
    buffer.drawCircle(indicatorX, indicatorY, 8, COLOR_GRAY);
  }

  buffer.setFont(FONT_4x6);
  buffer.setTextColor(lilka::colors::White);
  buffer.setCursor(centerX - 35, centerY + 70);
  buffer.print("A:L B:R St:Mid");
  buffer.setCursor(centerX - 30, centerY + 82);
  buffer.print("C:Up D:Down");
}

// Keyboard layouts
static const char *KB_DISPLAY_LOWER[5] = {
    "1234567890", "qwertyuiop", "asdfghjkl;", "zxcvbnm,./",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

static const char *KB_DISPLAY_UPPER[5] = {
    "!@#$%^&*()", "QWERTYUIOP", "ASDFGHJKL:", "ZXCVBNM<>?",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

static const char *KB_DISPLAY_SYMBOLS[5] = {
    "1234567890", "-=[]\\|`~  ", "!@#$%^&*()", "_+{}|~    ",
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A"};

// Special key labels: Ent, Tab, Esc, <-, ->, Up, Dn, Del, Hm, End
static const char *SPECIAL_KEY_LABELS[10] = {"Ent", "Tab", "Esc", "<-", "->",
                                             "Up",  "Dn",  "Del", "Hm", "End"};

void UI::drawKeyboardMode(const lilka::State &state, int cursorX, int cursorY,
                          int layer, const char *text) {
  buffer.setFont(FONT_6x13);
  buffer.setTextColor(lilka::colors::Orange);
  buffer.setCursor(5, 35);

  const char *layerName;
  switch (layer) {
  case 0:
    layerName = "abc";
    break;
  case 1:
    layerName = "ABC";
    break;
  case 2:
    layerName = "!@#";
    break;
  default:
    layerName = "abc";
    break;
  }
  buffer.print(STR_KEYBOARD_MODE);
  buffer.print(" [");
  buffer.print(layerName);
  buffer.print("]");

  // Text input field
  int textFieldY = 50;
  buffer.drawRect(5, textFieldY, width - 10, 20, lilka::colors::White);
  buffer.setFont(FONT_5x7);
  buffer.setTextColor(lilka::colors::Cyan);
  buffer.setCursor(8, textFieldY + 14);
  if (text && strlen(text) > 0) {
    int textLen = strlen(text);
    if (textLen > 40) {
      buffer.print("...");
      buffer.print(text + textLen - 37);
    } else {
      buffer.print(text);
    }
  }
  buffer.print("_");

  const char **currentLayout;
  switch (layer) {
  case 0:
    currentLayout = KB_DISPLAY_LOWER;
    break;
  case 1:
    currentLayout = KB_DISPLAY_UPPER;
    break;
  case 2:
    currentLayout = KB_DISPLAY_SYMBOLS;
    break;
  default:
    currentLayout = KB_DISPLAY_LOWER;
    break;
  }

  // Keyboard grid
  int kbStartX = 8;
  int kbStartY = 72;
  int keyW = 25;
  int keyH = 17;
  int gap = 1;

  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      int x = kbStartX + col * (keyW + gap);
      int y = kbStartY + row * (keyH + gap);

      char key = currentLayout[row][col];
      bool isSelected = (row == cursorY && col == cursorX);
      bool isSpecialKey = (key >= '\x01' && key <= '\x0A');

      uint16_t keyColor;
      if (isSelected) {
        keyColor = lilka::colors::Green;
      } else if (isSpecialKey) {
        keyColor = COLOR_NAVY;
      } else {
        keyColor = COLOR_DARK_GRAY;
      }
      buffer.fillRect(x, y, keyW, keyH, keyColor);
      buffer.drawRect(x, y, keyW, keyH,
                      isSelected ? lilka::colors::Yellow : COLOR_GRAY);

      buffer.setTextColor(isSelected ? lilka::colors::Black
                                     : lilka::colors::White);

      if (isSpecialKey) {
        buffer.setFont(FONT_4x6);
        int labelIdx = key - '\x01';
        if (labelIdx >= 0 && labelIdx < 10) {
          buffer.setCursor(x + 2, y + 12);
          buffer.print(SPECIAL_KEY_LABELS[labelIdx]);
        }
      } else if (key != ' ' && key != '\0') {
        buffer.setFont(FONT_6x13);
        buffer.setCursor(x + 8, y + 13);
        buffer.print(key);
      }
    }
  }

  // Control hints
  int btnY = kbStartY + 5 * (keyH + gap) + 1;
  buffer.setFont(FONT_4x6);

  buffer.setTextColor(state.a.pressed ? lilka::colors::Green
                                      : COLOR_LIGHT_GRAY);
  buffer.setCursor(12, btnY + 7);
  buffer.print("A:Type");

  buffer.setTextColor(state.b.pressed ? lilka::colors::Red : COLOR_LIGHT_GRAY);
  buffer.setCursor(55, btnY + 7);
  buffer.print("B:Bksp");

  buffer.setTextColor(state.c.pressed ? lilka::colors::Blue : COLOR_LIGHT_GRAY);
  buffer.setCursor(100, btnY + 7);
  buffer.print("C:");
  buffer.print(layerName);

  buffer.setTextColor(state.d.pressed ? lilka::colors::Yellow
                                      : COLOR_LIGHT_GRAY);
  buffer.setCursor(140, btnY + 7);
  buffer.print("D:Spc");

  buffer.setTextColor(state.start.pressed ? lilka::colors::Cyan
                                          : COLOR_LIGHT_GRAY);
  buffer.setCursor(180, btnY + 7);
  buffer.print("St:Ent");
}

void UI::drawPressedKeys(const lilka::State &state) {
  buffer.setFont(FONT_4x6);

  if (state.start.pressed || state.select.pressed) {
    buffer.setCursor(20, height - 25);
    if (state.start.pressed && state.select.pressed) {
      buffer.setTextColor(lilka::colors::Yellow);
      buffer.print(STR_HOLD_SWITCH);
    } else if (state.start.pressed) {
      buffer.setTextColor(lilka::colors::White);
      buffer.print(STR_START);
    } else {
      buffer.setTextColor(lilka::colors::White);
      buffer.print(STR_SELECT);
    }
  }
}

void UI::drawInstructions(DeviceMode mode) {
  buffer.setFont(FONT_4x6);
  buffer.setTextColor(COLOR_GRAY);
  buffer.setCursor(25, height - 8);
  buffer.print("Hold START+SELECT 3s to switch");
}

} // namespace ble_controller_app
