#include <Arduino.h>
#include "logger.h"

Logger::Logger() : buffer_(), index_(0) {
}

uint32_t Logger::LoggedPeriod() const {
  return (index_ > 0) ? (buffer_[index_ - 1].micros - buffer_[0].micros) : 0;
}

void Logger::Start(IMU6886* imu, uint32_t interval_us) {
  auto last = ::micros();
  for (auto& log : buffer_) {
    auto now = ::micros();
    while ((now - last) < interval_us) {
      now = ::micros();
    }

    float x, y, z;
    imu->get(&x, &y, &z);
    log.micros = now;
    log.x = static_cast<int16_t>(x * 100.F);
    log.y = static_cast<int16_t>(y * 100.F);
    log.z = static_cast<int16_t>(z * 100.F);

    last = now;
  }
  index_ = static_cast<int>(sizeof(buffer_)/sizeof(LOG));
}

void Logger::Flush(Stream* stream) {
  for (auto i = 0; i < index_; ++i) {
    const auto& log = buffer_[i];
    auto line = String(log.micros) + "," +
                String(log.x / 100.F) + "," +
                String(log.y / 100.F) + "," +
                String(log.z / 100.F);
    stream->println(line);
  }
  index_ = 0;
}
