#include "defines.h"
#include <Arduino.h>
#include <lilka.h>

#include "controller.h"
#include "fps.h"

using namespace ble_controller_app;

Controller *bleGController = nullptr;
FPS *uiFps = nullptr;

void setup() {
  lilka::begin();

  bleGController = new Controller();
  uiFps = new FPS(UI_FPS_WATCHER, UI_DELAY_MILLIS);

  if (!bleGController->start()) {
    lilka::serial_log("[%s] Failed to start BLE controller", LOG_TAG);
    return;
  }

  lilka::serial_log("[%s] BLE Controller started successfully", LOG_TAG);
}

void loop() {
  if (bleGController && bleGController->isActive()) {
    uiFps->onStartFrame();

    // Handle UI updates here
    // For now, just delay
    vTaskDelay(UI_DELAY_MILLIS / portTICK_PERIOD_MS);

    uiFps->onEndFrame();
    if (DEBUG) {
      uiFps->logEveryOneSecond();
    }
  } else {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
