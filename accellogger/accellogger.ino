#include <BluetoothSerial.h>
#include <M5StickC.h>
#include "imu6886.h"
#include "logger.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#  error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

namespace {

IMU6886 imu;
BluetoothSerial serialBT;
Logger logger;

void command(Stream* stream) {
  if (stream == nullptr || stream->available() == 0) return;

  const auto c = stream->read();
  switch (::tolower(c)) {
    case 'c':
      ::digitalWrite(10, LOW);
      imu.calibrate();
      ::digitalWrite(10, HIGH);
      break;
    case 'b':
      stream->println("begin");
      logger.Start(&imu, 1000, stream);
      stream->print("end (");
      stream->print(logger.LoggedPeriod());
      stream->println("us)");
      break;
    case 's':
      logger.Flush(stream);
      break;
    default:
      break;
  }
}

Stream* streams[2] = { &Serial, &serialBT };

}  // namespace

void setup() {
  M5.begin(false, false, true);
  serialBT.begin("M5Accel");
  ::pinMode(10, OUTPUT);

  imu.begin();
  imu.calibrate();
  ::digitalWrite(10, HIGH);
}

void loop() {
  for (auto& stream : streams) {
    command(stream);
  }
}
