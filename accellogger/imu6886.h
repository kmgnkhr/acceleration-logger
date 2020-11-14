#ifndef ACCELLOGGER_IMU6886_H_
#define ACCELLOGGER_IMU6886_H_

class IMU6886 {
 public:
  IMU6886();

  void begin();

  void calibrate();

  void get(float* x, float* y, float* z);

 private:
  float offset_[3];
};

#endif  // ACCELLOGGER_IMU6886_H_
