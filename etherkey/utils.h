#pragma once

#include <Print.h>
#include "usb_keyboard.c"

#ifndef HWSERIAL
#define HWSERIAL Serial
#endif

#define SerialPrintf(fmt, ...) SerialPrintfOut(HWSERIAL, 0, fmt, ##__VA_ARGS__)
#define SerialPrintfln(fmt, ...) SerialPrintfOut(HWSERIAL, 1, fmt, ##__VA_ARGS__)
void SerialPrintfOut(Print &output, bool newline, const char* fmt, ...) {
  char buff[128];
  va_list args;
  va_start (args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  va_end (args);
  if (newline)
    output.println(buff);
  else
    output.print(buff);
}

#define SerialClear() SerialClearOut(HWSERIAL)
void SerialClearOut(Print &output) {
  output.write(27);
  output.print("[2J");
  output.write(27);
  output.print("[H");
}

#define SerialDeleteChars(len) SerialDeleteCharsOut(HWSERIAL, len)
void SerialDeleteCharsOut(Print &output, int len) {
  char buff[7];
  output.write(27);
  snprintf(buff, sizeof(buff), "[%iD", len);
  output.print(buff);
  output.write(27);
  output.print("[0J");
}

#define SerialClearLine() SerialClearLineOut(HWSERIAL)
void SerialClearLineOut(Print &output) {
  output.write(27);
  output.print("[1G");
  output.write(27);
  output.print("[0J");
}

#define SerialAnsiEsc(esc) SerialAnsiEscOut(HWSERIAL, esc)
void SerialAnsiEscOut(Print &output, const char* seq) {
  char buff[10];
  snprintf(buff, sizeof(buff), "[%s", seq);
  output.write(27);
  output.print(buff);
}


// Compile time bernstein hash function
constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}
