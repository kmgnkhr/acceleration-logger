# acceleration-logger
Logging acceleration with M5StickC.

## How to use

1. Attach the M5StickC to the measurement target
1. Connect with a PC via serial port (USB or Bluetooth).
1. Measurement start (start by command via serial port).

* Open with 115200bps baud rate when connected via USB.

## Commands

### c : Calibrate

### b### : Start logging

The argument (###) specifies the sampling period(usec).  
When omitted, 1000us.

b3000 -> 3000us  
b500 -> 500us  
b -> 1000us(default)

Continue logging at the specified period until consumed 7000 pieces or enter 'e'.

### s : Output the log data.

Output the log data in CSV format.  
Sampling time (usec), X-axis acceleration(G), Y-axis acceleration, Z-axis acceleration
