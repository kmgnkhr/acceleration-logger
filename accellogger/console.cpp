#include "console.h"
#include <Arduino.h>

Console::Console(IMU6886* imu, Stream* stream)
: imu_(imu), stream_(stream), show_(false) {
}

void Console::begin() {
  show_ = false;

  if (imu_) {
    imu_->begin();
    imu_->calibrate();
  }
}

bool Console::loop() {
  if (command()) {
    return true;
  }
  if (!show_) {
    return false;
  }
  float x, y, z;
  imu_->get(&x, &y, &z);
  auto line = String(::millis(), DEC) + "," +
              String(x) + "," + String(y) + "," + String(z);
  stream_->println(line);
  return false;
}

bool Console::command() {
  if (stream_ == nullptr || stream_->available() == 0) return false;

  auto c = stream_->read();
  switch (::tolower(c)) {
    case 'c':
      imu_->calibrate();
      return true;
    case 'b':
      stream_->println("begin");
      show_ = true;
      return true;
    case 'e':
      stream_->println("end");
      show_ = false;
      return true;
    default:
      break;
  }
  return false;
}
