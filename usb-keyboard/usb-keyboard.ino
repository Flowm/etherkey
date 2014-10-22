#define HWSERIAL Serial1

#include "utils.h"

#define KBD_BUFFSZ 200

char inChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int in_mode = 1;


void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    // Switch mode via control character
    //TODO: Use Prefix + Number/ Character
    if (inChar < 10) {
      in_mode = inChar;
      SerialClear();
      SerialPrintf("Switching to mode %i", in_mode);
      return;
    }

    switch(in_mode) {
      case 1: //BUFFERED MODE
        if (inChar == '\n' || inChar == '\r' || kbd_idx >= KBD_BUFFSZ-1) {
          HWSERIAL.println();
          kbd_buff[kbd_idx++] = '\0';
          parse(kbd_buff);
          kbd_idx = 0;
        } else {
          kbd_buff[kbd_idx++] = inChar;
          HWSERIAL.write(inChar);
        }

        break;
      case 2: //INTERACTIVE MODE
        HWSERIAL.write(inChar);
        break;

      case 3: //DEBUG MODE
        SerialPrintf("Recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
        break;
    }
  }
}


void parse(char * str) {
}
