#ifndef ACCELLOGGER_CONSOLE_H_
#define ACCELLOGGER_CONSOLE_H_

#include <Stream.h>
#include "imu6886.h"

class Console  {
 private:
  IMU6886* imu_;
  Stream* stream_;
  bool show_;
 public:
  Console(IMU6886* imu, Stream* stream);

  void begin();
  bool loop();

  void print_log();

 private:
  bool command();
};

#endif  // ACCELLOGGER_CONSOLE_H_
