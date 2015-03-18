// Jankey device-driver-like-thing for the serial port.

#ifndef HAL_SERIAL_PORT_H_
#define HAL_SERIAL_PORT_H_

#include "klib/types.h"
#include "klib/typeprinter.h"

namespace hal {

// See: http://wiki.osdev.org/Serial_Ports
class SerialPort {
 private:
  // Do not construct. Static utility class.
  SerialPort();

 public:
  static void WriteByte(byte b);

 private:
  static bool IsInitialized();
  static void Initialize();

  static bool IsTransmitEmpty();

  static bool initialized_;
};

// IOutputFn that writes to the serial port.
class SerialPortOutputFn : public klib::IOutputFn {
 public:
  virtual void Print(char c);
};

}  // namespace hal

#endif  // HAL_SERIAL_PORT_H_
