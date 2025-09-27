#include <Arduino.h>
#include <lilka.h>
#include "defines.h"

#ifdef GAMEPAD
#include "controller.h"
#include "fps.h"

using namespace ble_gamepad_app;

Controller* bleGamepadController = nullptr;
FPS* uiFps = nullptr;

void setup() {
    lilka::begin();
    
    bleGamepadController = new Controller();
    uiFps = new FPS(UI_FPS_WATCHER, UI_DELAY_MILLIS);
    
    if (!bleGamepadController->start()) {
        lilka::serial_log("[%s] Failed to start BLE controller", LOG_TAG);
        return;
    }
    
    lilka::serial_log("[%s] BLE Gamepad started successfully", LOG_TAG);
}

void loop() {
    if (bleGamepadController && bleGamepadController->isActive()) {
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

#endif // GAMEPAD

#ifdef MOUSE_KEYBOARD
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <lilka.h>

void setup() {
    lilka::begin();

    lilka::serial_log("[%s] BLE Mouse/Keyboard started successfully", LOG_TAG);
}

void loop() {

}
#endif // MOUSE_KEYBOARD