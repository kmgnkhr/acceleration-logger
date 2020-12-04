#include <Arduino.h>
#include "logger.h"

Logger::Logger() : buffer_(), index_(0) {
}

uint32_t Logger::LoggedPeriod() const {
  return (index_ > 0) ? (buffer_[index_ - 1].micros - buffer_[0].micros) : 0;
}

namespace {
bool isCancellationRequest(Stream* stream) {
  if (stream != nullptr && stream->available()) {
    if (stream->read() == 'e') return true;
  }
  return false;
}
}  // namespace

void Logger::Start(IMU6886* imu, uint32_t interval_us, Stream* stream) {
  index_ = 0;
  auto last = ::micros();
  const auto start = last;
  for (auto& log : buffer_) {
    auto now = ::micros();
    while ((now - last) < interval_us) {
      if (isCancellationRequest(stream)) {
        return;
      }
      now = ::micros();
    }

    float x, y, z;
    imu->get(&x, &y, &z);
    log.micros = now - start;
    log.x = static_cast<int16_t>(x * 10000.F);
    log.y = static_cast<int16_t>(y * 10000.F);
    log.z = static_cast<int16_t>(z * 10000.F);

    last = now;
    ++index_;
  }
}

void Logger::Flush(Stream* stream) {
  for (auto i = 0; i < index_; ++i) {
    const auto& log = buffer_[i];
    auto line = String(log.micros) + "," +
                String(log.x / 10000.F, 4) + "," +
                String(log.y / 10000.F, 4) + "," +
                String(log.z / 10000.F, 4);
    stream->println(line);
  }
  index_ = 0;
}

uint32_t Logger::CalcRate() const {
  return (index_ <= 0) ? 0 : buffer_[index_ - 1].micros / index_;
}
