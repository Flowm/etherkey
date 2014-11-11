#define HWSERIAL Serial1

#include "utils.h"

#define KBD_BUFFSZ 200
#define PREFIX 17 // CTRL-Q
#define MYDEBUG

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
          c_parse(kbd_buff);
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

      case 4: //VERBOSE DEBUG MODE
        SerialPrintfln("Recv -> Character: %c - ASCII-Code: %3i", inChar, inChar);

        char keycode_b[33];
        char key_b[33];
        char mod_b[33];

        KEYCODE_TYPE keycode = unicode_to_keycode(inChar);
        itoa(keycode, keycode_b, 2);

        uint8_t key = keycode_to_key(keycode);
        itoa(key, key_b, 2);

        uint8_t mod = keycode_to_modifier(keycode);
        itoa(mod, mod_b, 2);

        SerialPrintfln("keycode: %3i = %08s | key: %3i = %08s | mod: %3i = %08s", keycode, keycode_b, key, key_b, mod, mod_b);
        break;
    }
  }
}


KEYCODE_TYPE escape_sequence_to_keycode() {
  KEYCODE_TYPE keycode = 0;
  char in_ascii;

  in_ascii = HWSERIAL.peek();
  if (in_ascii == 91) {
    HWSERIAL.read();
    in_ascii = HWSERIAL.peek();
    if (in_ascii >= 65 && in_ascii <= 68) {
      HWSERIAL.read();
      if (in_ascii == 65)
        keycode = KEY_UP;
      if (in_ascii == 66)
        keycode = KEY_DOWN;
      if (in_ascii == 67)
        keycode = KEY_RIGHT;
      if (in_ascii == 68)
        keycode = KEY_LEFT;
    } else if (in_ascii == 51) {
      in_ascii = HWSERIAL.peek();
      if (in_ascii == 126) {
        HWSERIAL.read();
        keycode = KEY_DELETE;
      }
    }
  }
  return keycode;
}

void interactive_send(char key) {
  //TODO: Print on Serial
  //TODO: Make it work on Windows
  KEYCODE_TYPE keycode = 0;

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
        keycode = escape_sequence_to_keycode();
        if (keycode)
          SendKey(keycode);
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


// Command mode functions
void c_parse(char * str) {
  int state = 0;
  char * pch;

  pch = strtok(str," ");
#ifdef MYDEBUG
  SerialPrintfln("\t%-10s -> %x", pch, str2int(pch));
#endif

  switch (str2int(pch)) {
    case str2int("SendRaw"):
      // Send the rest of the line literally
      pch = strtok (NULL,"");
      if (pch != NULL)
        c_sendraw(pch);
      break;

    case str2int("Send"):
      // Send the rest of the line (and parse special characters)
      pch = strtok (NULL,"");
      if (pch != NULL)
        c_send(pch);
      break;

    case str2int("UnicodeLinux"):
    case str2int("UCL"):
      // Send a single unicode character (on Linux)
      pch = strtok (NULL,"");
      if (pch != NULL)
        c_unicode(pch, true);
      break;

    case str2int("UnicodeWindows"):
    case str2int("UCW"):
      // Send a single unicode character (on Windows)
      pch = strtok (NULL,"");
      if (pch != NULL)
        c_unicode(pch, false);
      break;

    case str2int("Sleep"):
      // Sleep a certain amount of time in ms
      pch = strtok (NULL,"");
      if (pch != NULL)
        c_sleep(pch);
      break;

    case str2int("Enter"):
      // Send the Enter Key
      //TODO
      break;

    case str2int("Help"):
      // Display a informative help message
      //TODO
      break;

    default:
      // Handle unknown command
      //TODO
      break;
  }
}

void c_sendraw(char * pch) {
  char * c = pch;

  while (*c != NULL) {
#ifdef MYDEBUG
    SerialPrintfln("\tWriting %c via USB", c);
#endif
    //XXX
    c++;
  }
}

void c_send(char * pch) {
  char * c = pch;
  int modifier = 0;

  while (*c != NULL) {
    switch (*c) {
      case '!':
        // ALT
        modifier |= MODIFIERKEY_ALT;
        break;
      case '+':
        // SHIFT
        modifier |= MODIFIERKEY_SHIFT;
        break;
      case '^':
        // CTRL
        modifier |= MODIFIERKEY_CTRL;
        break;
      case '#':
        // WIN
        modifier |= MODIFIERKEY_GUI;
        break;
      default:
        KEYCODE_TYPE keycode = unicode_to_keycode(*c);
        uint8_t key = keycode_to_key(keycode);
        uint8_t mod = keycode_to_modifier(keycode) | modifier;

#ifdef MYDEBUG
        SerialPrintfln("Writing %c via USB. Keycode: %3i", *c, key);
#endif
        usb_keyboard_press(key, mod);
        break;
    }
    c++;
  }
}

void c_unicode(char * pch, bool linux) {
  //XXX
}

void c_sleep(char * pch) {
  int time = atoi(pch);
#ifdef MYDEBUG
  SerialPrintfln("\tSleep %i ms", time);
#endif
  //XXX
}
