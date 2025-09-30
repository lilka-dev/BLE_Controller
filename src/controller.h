#pragma once

#include "fps.h"
#include "timer.h"
#include <BleController.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#define NUM_OF_BUTTONS 8

namespace ble_controller_app {

class Controller : public BleController {
public:
  Controller();
  ~Controller();

  bool start();
  bool isActive();
  void stop();

private:
  static void controllerTimerCallback(TimerHandle_t xTimer);
  void updateControllerState();
  bool updateBatteryLevel();
  bool updateButtons();

  bool active;
  TimerHandle_t controllerTimer;
  FPS mainLoopFps;
  int batteryLevel;
  Timer checkBatteryLevelTimer;
};

} // namespace ble_controller_app
