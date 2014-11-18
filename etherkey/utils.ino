#include "utils.h"

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

void SerialClearOut(Print &output) {
  SerialAnsiEsc("2J");
  SerialAnsiEsc("H");
}

void SerialDeleteCharsOut(Print &output, int len) {
  for (int i = 0; i < len; i++) {
    SerialAnsiEsc("D");
  }
  SerialAnsiEsc("0J");
}

void SerialClearLineOut(Print &output) {
  SerialAnsiEsc("1G");
  SerialAnsiEsc("0J");
}

void SerialAnsiEscOut(Print &output, const char* seq) {
  char buff[10];
  snprintf(buff, sizeof(buff), "[%s", seq);
  output.write(27);
  output.print(buff);
}
