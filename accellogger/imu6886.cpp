#include "imu6886.h"
#include <M5StickC.h>

#undef MPU6886

IMU6886::IMU6886() : offset_() {
}

void IMU6886::begin() {
  M5.IMU.Init();
  Wire1.setClock(10000000UL);
  M5.Mpu6886.SetAccelFsr(MPU6886::AFS_4G);
}

void IMU6886::calibrate() {
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
}

void IMU6886::get(float* x, float* y, float* z) {
  M5.IMU.getAccelData(x, y, z);
  if (x) *x -= offset_[0];
  if (y) *y -= offset_[1];
  if (z) *z -= offset_[2];
}
