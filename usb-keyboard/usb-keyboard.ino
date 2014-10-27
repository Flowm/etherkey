#define HWSERIAL Serial1

#include "utils.h"

#define KBD_BUFFSZ 200

char inChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int in_mode = 2;


void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    // Switch mode via control character
    //TODO: Use Prefix + Number/ Character
    //if (inChar <= 7) {
    //  in_mode = inChar;
    //  SerialClear();
    //  SerialPrintfln("Switching to mode %i", in_mode);
    //  return;
    //}
    
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
	interactive_echo(inChar);
        break;

      case 3: //DEBUG MODE
        SerialPrintfln("Recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
        break;
    }
  }
}

void interactive_echo(char str) {
  //TODO: Print on Serial
  //TODO: Make it work on Windows
  char key;
  if (str <= 26) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.set_key1(str+3);
    Keyboard.send_now();
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  } else if (str == 27) {
    if (HWSERIAL.read() == 91) {
      key = HWSERIAL.read();
      if (65 <= key <= 68) {
	if (key == 65)
          key = KEY_UP;
	if (key == 66)
          key = KEY_DOWN;
	if (key == 67)
          key = KEY_RIGHT;
	if (key == 68)
          key = KEY_LEFT;
        Keyboard.set_key1(key);
        Keyboard.send_now();
        Keyboard.set_modifier(0);
        Keyboard.set_key1(0);
        Keyboard.send_now();
      }
    }
  } else {
    Keyboard.print(str);
  }

  /* switch(str) {
    case 3:
      Keyboard.set_modifier(MODIFIERKEY_CTRL);
      Keyboard.set_key1(6);
      HWSERIAL.write("^C\r\n");
      Keyboard.send_now();
      break;
    case 4:
      Keyboard.set_modifier(MODIFIERKEY_CTRL);
      Keyboard.set_key1(KEY_D);
      HWSERIAL.write("^D\r\n");
      Keyboard.send_now();
      break;
    case 8:
      Keyboard.set_modifier(MODIFIERKEY_CTRL);
      Keyboard.set_key1(KEY_H);
      Keyboard.send_now();
      break;
    case 13:
      Keyboard.set_modifier(MODIFIERKEY_CTRL);
      Keyboard.set_key1(KEY_M);
      Keyboard.send_now();
      break;
    case 37:
      Keyboard.set_key1(KEY_LEFT);
      Keyboard.send_now();
      break;
    default:
      Keyboard.print(str);
      break;
  } */
}

void parse(char * str) {
}
