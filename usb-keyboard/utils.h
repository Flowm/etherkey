#pragma once

#include <Print.h>
#include "usb_keyboard.c"

#ifndef HWSERIAL
#define HWSERIAL Serial
#endif

#define SerialPrintf(fmt, ...) SerialPrintfOut(HWSERIAL, 0, fmt, ##__VA_ARGS__)
#define SerialPrintfln(fmt, ...) SerialPrintfOut(HWSERIAL, 1, fmt, ##__VA_ARGS__)
void SerialPrintfOut(Print &output, bool newline, char * fmt, ...) {
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

#define SerialWriteEsc(key) SerialWriteEscOut(HWSERIAL, key)
int SerialWriteEscOut(Print &output, char key) {
  //Writes char on serial, but handles special characters (backspace, arrow keys...) correctly
  //Returns 0 for normal characters, 1 for special characters
  //TODO
  output.write(key);
  return 0;
}

void SendKeyNow(char key) {
  Keyboard.set_key1(key);
  Keyboard.send_now();
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
}

#define SendKey(key) SendKeyOut(HWSERIAL, key)
void SendKeyOut(Print &output, char key) {
  SerialWriteEscOut(output, key);
  SendKeyNow(key);
}
