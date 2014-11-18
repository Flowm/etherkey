#include "usb-keyboard.h"

char inChar;
char peekChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int crs_idx = 0;

int in_mode = 1;
enum mode {INVALID, COMMAND, INTERACTIVE, DEBUG};
const char* mode_strings[] = {"invalid", "command", "interactive", "debug"};
const char* selectMode = "Select Inputmode: [1] Command - [2] Interactive - [3] Debug";

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
        command_mode(inChar);
        break;

      case INTERACTIVE:
        interactive_mode(inChar);
        break;

      case DEBUG:
        char keycode_b[33];
        char key_b[33];
        char mod_b[33];

        KEYCODE_TYPE keycode = unicode_to_keycode(inChar);
        itoa(keycode, keycode_b, 2);
        uint8_t key = keycode_to_key(keycode);
        itoa(key, key_b, 2);
        uint8_t mod = keycode_to_modifier(keycode);
        itoa(mod, mod_b, 2);

        SerialPrintfln("Keycode: %3i = %08s | Key: %3i = %08s | Mod:%2i | ASCII: %3i = %c", keycode, keycode_b, key, key_b, mod, inChar, inChar);
        break;
    }
  }
}
