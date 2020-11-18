#include <BluetoothSerial.h>
#include "imu6886.h"
#include "logger.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#  error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

namespace {

IMU6886 imu;
BluetoothSerial serialBT;
Logger logger;

int read_argument(Stream* stream) {
  String str = "";
  while (true) {
    while (!stream->available()) {}
    auto c = stream->read();
    stream->print(static_cast<char>(c));
    if (c == 0xa || c == 0xd) break;
    str += static_cast<char>(c);
  }
  auto i = str.toInt();
  return i > 0 ? i : 1000;
}

void start(Stream* stream) {
  auto i = read_argument(stream);
  stream->print("begin (");
  stream->print(i);
  stream->println("us)");
  logger.Start(&imu, i, stream);
  stream->print("end (");
  stream->print(logger.LoggedPeriod());
  stream->println("us)");
}

void command(Stream* stream) {
  if (stream == nullptr || stream->available() == 0) return;

  const auto c = stream->read();
  stream->print(static_cast<char>(c));
  switch (::tolower(c)) {
    case 'c':
      read_argument(stream);
      ::digitalWrite(10, LOW);
      imu.calibrate();
      ::digitalWrite(10, HIGH);
      break;
    case 'b':
      ::digitalWrite(10, LOW);
      start(stream);
      ::digitalWrite(10, HIGH);
      break;
    case 's':
      read_argument(stream);
      logger.Flush(stream);
      break;
    default:
      break;
  }
}

Stream* streams[2] = { &Serial, &serialBT };

}  // namespace

void setup() {
  ::pinMode(10, OUTPUT);
  serialBT.begin("M5Accel");
  Serial.begin(115200);
  Serial.flush();
  ::delay(50);

  imu.begin();
  imu.calibrate();
  ::digitalWrite(10, HIGH);
}

void loop() {
  for (auto& stream : streams) {
    command(stream);
  }
}
