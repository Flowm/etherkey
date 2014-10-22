#include <Print.h>
#include <avr/pgmspace.h>

#pragma once

#ifndef HWSERIAL
#define HWSERIAL Serial
#endif

#define SerialPrintf(format, ...) SerialPrintfOut(HWSERIAL, PSTR(format), ##__VA_ARGS__)
void SerialPrintfOut(Print &output, char * fmt, ...) {
  char buff[128];
  va_list args;
  va_start (args, fmt);
  vsnprintf_P(buff, sizeof(buff), fmt,args);
  va_end (args);
  buff[sizeof(buff)/sizeof(buff[0])-1] = '\0';
  output.print(buff);
}

#define SerialClear() SerialClearOut(HWSERIAL)
void SerialClearOut(Print &output) {
  output.write(27);
  output.print("[2J");
  output.write(27);
  output.print("[H");
}
