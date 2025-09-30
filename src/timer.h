#pragma once

namespace ble_controller_app {

class Timer {
public:
  explicit Timer(int periodMillis);
  static unsigned long getTime();
  unsigned long go();
  bool isTime(bool triggerOnFirstCall = false);
  bool isTimeOnFirstCall();

private:
  int period;
  unsigned long start;
};

} // namespace ble_controller_app