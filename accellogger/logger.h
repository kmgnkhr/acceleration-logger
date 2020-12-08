#ifndef ACCELLOGGER_LOGGER_H_
#define ACCELLOGGER_LOGGER_H_

#include <Stream.h>
#include <stdint.h>
#include "imu6886.h"

class Logger {
  struct LOG {
    uint16_t micros;
    int16_t x, y, z;
  };
  LOG* buffer_;
  int index_;
  uint32_t interval_us_;

 public:
  Logger();

  void Start(IMU6886* imu, uint32_t interval_us, Stream* stream);
  void Show(Stream* stream) const;
  void Validate(Stream* stream) const;
  int SamplingCount() const;

 private:
  Logger(const Logger&);
  void operator=(const Logger&);
};

#endif  // ACCELLOGGER_LOGGER_H_
