#define HWSERIAL Serial1

#include "utils.h"

#define KBD_BUFFSZ 200
#define PREFIX 17 // CTRL-Q

char inChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;

int in_mode = 1;
bool switch_mode = false;


void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    if (inChar == PREFIX) {
      switch_mode = true;
      SerialPrintf("Select Mode: ");
      return;
    }
    if (switch_mode) {
      if (inChar >= 49 && inChar <= 51 ) {
        in_mode = inChar - 48;
      } else {
        in_mode = 3;
      }
      SerialClear();
      SerialPrintfln("Switching to mode %i", in_mode);
      switch_mode = false;
      return;
    }


    switch(in_mode) {
      case 1: //COMMAND MODE
        if (inChar == '\n' || inChar == '\r' || kbd_idx >= KBD_BUFFSZ-1) {
          SerialPrintfln("");
          kbd_buff[kbd_idx++] = '\0';
          parse(kbd_buff);
          kbd_idx = 0;
        } else {
          //TODO: React to special characters (backspace, arrow keys...)
          kbd_buff[kbd_idx++] = inChar;
          HWSERIAL.write(inChar);
        }
        break;

      case 2: //INTERACTIVE MODE
        interactive_echo(inChar);
        break;

      case 3: //DEBUG MODE
        SerialPrintfln("Recv -> Character: %c - ASCII-Code: %3i - USB-Scancode: %3i", inChar, inChar, unicode_to_keycode(inChar));
        //SerialPrintfln("Recv -> ASCII-Code:: %3i", inChar);
        break;
    }
  }
}


int parse_escape() {
  char key;
  key = HWSERIAL.read();
  if (key == 91) {
    key = HWSERIAL.read();
    if (65 <= key && key <= 68) {
      if (key == 65)
        key = KEY_UP;
      if (key == 66)
        key = KEY_DOWN;
      if (key == 67)
        key = KEY_RIGHT;
      if (key == 68)
        key = KEY_LEFT;
      SendKey(key);
    } else if (key == 51) {
      key = HWSERIAL.peek();
      if (key == 126) {
        key = HWSERIAL.read();
        SendKey(KEY_DELETE);
      }
    }
  }
  return 0; //TODO: Error-Codes would be nice
}

void interactive_echo(char key) {
  //TODO: Print on Serial
  //TODO: Make it work on Windows

  switch(key) {
    case 10:
    case 13:
      SendKey(KEY_ENTER);
      break;

    case 8:
    case 127:
      SendKey(KEY_BACKSPACE);
      break;

    case 9:
      SendKey(KEY_TAB);
      break;

    case 27:
      key = HWSERIAL.peek();
      if(key == 255) {
        SendKey(KEY_ESC);
      } else {
        parse_escape();
      }
      break;

    default:
      if (key <= 26) {
        Keyboard.set_modifier(MODIFIERKEY_CTRL);
        SendKey(key+3);
      } else {
        Keyboard.print(key);
      }
      break;
  }
}

void parse(char * str) {
}
