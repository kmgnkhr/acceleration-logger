#include <Arduino.h>
#include "logger.h"

Logger::Logger() : buffer_(nullptr), index_(0) {
}

namespace {
bool isCancellationRequest(Stream* stream) {
  if (stream != nullptr && stream->available()) {
    if (stream->read() == 'e') return true;
  }
  return false;
}

const uint32_t kMask = 10000;

}  // namespace

void Logger::Start(IMU6886* imu, uint32_t interval_us, Stream* stream) {
  if (buffer_ != nullptr) {
    delete [] buffer_;
  }
  const auto count = ::heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)
                                                             / sizeof(LOG);
  buffer_ = new LOG[count];
  index_ = 0;
  const auto start = ::micros();
  auto next = start;
  for (auto i = 0; i < count; ++i) {
    auto& log = buffer_[i];
    auto now = ::micros();
    while (now < next) {
      if (isCancellationRequest(stream)) {
        return;
      }
      now = ::micros();
    }

    float x, y, z;
    imu->get(&x, &y, &z);
    log.micros = (now - start) % kMask;
    log.x = static_cast<int16_t>(x * 10000.F);
    log.y = static_cast<int16_t>(y * 10000.F);
    log.z = static_cast<int16_t>(z * 10000.F);

    next += interval_us;
    ++index_;
  }
}

void Logger::Flush(Stream* stream) {
  auto offset = 0;
  auto last = 0;
  for (auto i = 0; i < index_; ++i) {
    const auto& log = buffer_[i];
    if ((log.micros - last) < 0) offset += kMask;
    last = log.micros;
    auto line = String(log.micros + offset) + "," +
                String(log.x / 10000.F, 4) + "," +
                String(log.y / 10000.F, 4) + "," +
                String(log.z / 10000.F, 4);
    stream->println(line);
  }
  index_ = 0;
}

int Logger::SamplingCount() const {
  return (index_ >= 0) ? index_ : 0;
}
