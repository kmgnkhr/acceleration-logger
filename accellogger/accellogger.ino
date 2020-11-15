#include <BluetoothSerial.h>
#include <M5StickC.h>
#include "imu6886.h"
#include "console.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#  error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

namespace {

IMU6886 imu;
BluetoothSerial serialBT;
Console console(&imu, &serialBT);
// Console console(&imu, &Serial);

}  // namespace

void setup() {
  M5.begin();
  serialBT.begin("M5Accel");
  console.begin();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    imu.calibrate();
    return;
  }

  if (console.loop()) {
    return;
  }
}
