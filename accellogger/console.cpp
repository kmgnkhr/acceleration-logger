#include "console.h"
#include <Arduino.h>

Console::Console(IMU6886* imu, Stream* stream)
: imu_(imu), stream_(stream), show_(false) {
}

namespace {

const int QUEUE_LENGTH = 1;
hw_timer_t* timer = nullptr;
QueueHandle_t xQueue;
TaskHandle_t taskHandle;

void IRAM_ATTR onTimer() {
  int8_t data;
  xQueueSendToFrontFromISR(xQueue, &data, 0);
}

void task_main(void* pvParameters) {
  auto console = static_cast<Console*>(pvParameters);
  while (true) {
    int8_t data;
    xQueueReceive(xQueue, &data, portMAX_DELAY);
    console->print_log();
  }
}

void kickStartLoggerTask(void* pvParameters) {
  xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(int8_t));
  xTaskCreateUniversal(task_main, "logger", 8192,
                       pvParameters, 5, &taskHandle, APP_CPU_NUM);

  timer = timerBegin(0, getApbFrequency() / 1000000, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 5 * 1000, true);
  timerAlarmEnable(timer);
}

}  // namespace

void Console::begin() {
  show_ = false;

  if (imu_) {
    imu_->begin();
    imu_->calibrate();
  }

  kickStartLoggerTask(this);
}

bool Console::loop() {
  return command();
}

bool Console::command() {
  if (stream_ == nullptr || stream_->available() == 0) return false;

  auto c = stream_->read();
  switch (::tolower(c)) {
    case 'c':
      imu_->calibrate();
      return true;
    case 'b':
      stream_->println("begin");
      show_ = true;
      return true;
    case 'e':
      stream_->println("end");
      show_ = false;
      return true;
    default:
      break;
  }
  return false;
}

void Console::print_log() {
  if (!show_) {
    return;
  }
  float x, y, z;
  imu_->get(&x, &y, &z);
  auto line = String(::millis(), DEC) + "," +
              String(x) + "," + String(y) + "," + String(z);
  stream_->println(line);
}
