#include "hal/serial_port.h"

#include "klib/types.h"
#include "sys/io.h"

// See: http://wiki.osdev.org/Serial_Ports

namespace {
const uint32 kCOM1 = 0x03F8;
}  // anonymous namespace

namespace hal {

// Initialize static fields.
bool SerialPort::initialized_ = false;

SerialPort::SerialPort() {}

void SerialPort::WriteByte(byte b) {
  if (!IsInitialized()) {
    Initialize();
  }

  while (!IsTransmitEmpty()) {
    // Wait until the buffer is clear.
  }
  outb(kCOM1, (uint32) b);
}

bool SerialPort::IsInitialized() {
  return initialized_;
}

void SerialPort::Initialize() {
  outb(kCOM1 + 1, 0x00);  // Disable all interrupts
  outb(kCOM1 + 3, 0x80);  // Enable DLAB (set baud rate divisor)
  outb(kCOM1 + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
  outb(kCOM1 + 1, 0x00);  //                  (hi byte)
  outb(kCOM1 + 3, 0x03);  // 8 bits, no parity, one stop bit
  outb(kCOM1 + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
  outb(kCOM1 + 4, 0x0B);  // IRQs enabled, RTS/DSR set

  initialized_ = true;
}

bool SerialPort::IsTransmitEmpty() {
  return ((inb(kCOM1 + 5) & 0x20) != 0);
}

void SerialPortOutputFn::Print(char c) {
  SerialPort::WriteByte(byte(c));
}

}  // namespace hal
