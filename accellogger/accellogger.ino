#include <BluetoothSerial.h>
#include <M5StickC.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#  error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

namespace {

class IMU6886 {
 private:
  float offset_[3];

 public:
  IMU6886() : offset_() {}

  void begin() {
    M5.IMU.Init();
  }

  void calibrate() {
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

  void get(float* x, float* y, float* z) {
    M5.IMU.getAccelData(x, y, z);
    if (x) *x -= offset_[0];
    if (y) *y -= offset_[1];
    if (z) *z -= offset_[2];
  }
};

IMU6886 imu;
BluetoothSerial SerialBT;

}  // namespace

void setup() {
  M5.begin();
  SerialBT.begin("M5Accel");

  ::pinMode(10, OUTPUT);

  imu.begin();
  imu.calibrate();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    imu.calibrate();
    return;
  }

  float x, y, z;
  imu.get(&x, &y, &z);

  auto line = String(::millis(), DEC) + "," +
              String(x) + "," + String(y) + "," + String(z);

  Serial.println(line);
  SerialBT.println(line);

  ::delay(100);
}
