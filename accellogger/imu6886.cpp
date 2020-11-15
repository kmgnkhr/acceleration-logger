#include "imu6886.h"
#include <M5StickC.h>

IMU6886::IMU6886() : offset_() {
}

void IMU6886::begin() {
  M5.IMU.Init();
  ::pinMode(10, OUTPUT);
}

void IMU6886::calibrate() {
  ::digitalWrite(10, LOW);
  float sum[3] = {0};

  const auto counter = 500;
  for (auto i = 0; i < counter; ++i) {
    float acc[3];
    M5.IMU.getAccelData(&acc[0], &acc[1], &acc[2]);
    sum[0] += acc[0];
    sum[1] += acc[1];
    sum[2] += acc[2];
    ::delay(2);
  }

  for (auto& o : offset_) {
    o = sum[&o - &offset_[0]] / counter;
  }
  offset_[2] -= 1.F;
  ::digitalWrite(10, HIGH);
}

void IMU6886::get(float* x, float* y, float* z) {
  M5.IMU.getAccelData(x, y, z);
  if (x) *x -= offset_[0];
  if (y) *y -= offset_[1];
  if (z) *z -= offset_[2];
}