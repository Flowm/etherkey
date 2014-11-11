#define HWSERIAL Serial1

#include "utils.h"

#define KBD_BUFFSZ 200
#define PREFIX 17 // CTRL-Q

char inChar;
char peekChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;

int in_mode = 1;
enum mode {INVALID, COMMAND, INTERACTIVE, DEBUG};
const char * mode_strings[] = {"invalid", "command", "interactive", "debug"};
char * selectMode = "Select Inputmode: [1] Command - [2] Interactive - [3] Debug";

void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
  SerialClear();
  SerialPrintfln("Switching to %s mode", mode_strings[in_mode]);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    if (inChar == PREFIX) {
      SerialPrintf("%s", selectMode);
      do {
        while (HWSERIAL.available() == 0) {
          delay(100);
        }
        peekChar = HWSERIAL.peek();
        if (peekChar == PREFIX) {
          inChar = HWSERIAL.read();
          Keyboard.print(inChar);
          SerialDeleteChars(strlen(selectMode));
          return;
        } else if (peekChar > '0' && peekChar < ('0'+sizeof(mode_strings)/sizeof(char*))) {
          inChar = HWSERIAL.read();
          in_mode = inChar - '0';
          SerialClear();
          SerialPrintfln("Switching to %s mode", mode_strings[in_mode]);
          return;
        } else if (peekChar == 27) {
          inChar = HWSERIAL.read();
          SerialDeleteChars(strlen(selectMode));
          return;
        } else {
          inChar = HWSERIAL.read();
          SerialDeleteChars(strlen(selectMode));
          SerialPrintf("%s", selectMode);
        }
      } while (peekChar != 27 || peekChar != PREFIX || (peekChar >= 49 && peekChar <= 51));
    }


    switch(in_mode) {
      case COMMAND:
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

      case INTERACTIVE:
        interactive_send(inChar);
        break;

      case DEBUG:
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

void interactive_send(char key) {
  //TODO: Print on Serial
  //TODO: Make it work on Windows

  switch(key) {
    case 10:  //LF
    case 13:  //CR
      SendKey(KEY_ENTER);
      break;

    case 8:   //BS
    case 127: //DEL
      SendKey(KEY_BACKSPACE);
      break;

    case 9:   //HT
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
        SerialWriteEsc(key);
      }
      break;
  }
}

void parse(char * str) {
}
