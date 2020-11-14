#include <BluetoothSerial.h>
#include <M5StickC.h>
#include "imu6886.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#  error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

namespace {

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
