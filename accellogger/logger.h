#ifndef ACCELLOGGER_LOGGER_H_
#define ACCELLOGGER_LOGGER_H_

#include <Stream.h>
#include <stdint.h>
#include "imu6886.h"

class Logger {
  struct LOG {
    uint32_t micros;
    int16_t x, y, z;
  } buffer_[7000];
  int index_;

 public:
  Logger();

  uint32_t LoggedPeriod() const;
  void Start(IMU6886* imu, uint32_t interval_us);
  void Flush(Stream* stream);

 private:
  Logger(const Logger&);
  void operator=(const Logger&);
};

#endif  // ACCELLOGGER_LOGGER_H_
