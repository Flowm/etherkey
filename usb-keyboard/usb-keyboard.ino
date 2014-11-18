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

// Util functions
uint16_t escape_sequence_to_keycode(char key) {
  char in_ascii = key;
  uint16_t keycode = 0;

  if (in_ascii == 91) {
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
      HWSERIAL.read();
      in_ascii = HWSERIAL.peek();
      if (in_ascii == 126) {
        HWSERIAL.read();
        keycode = KEY_DELETE;
      }
    }
  }
  return keycode;
}

uint16_t special_char_to_keycode(char in_ascii) {
  uint16_t keycode = 0;

  switch(in_ascii) {
    case 10:  //LF
    case 13:  //CR
      keycode = KEY_ENTER;
      break;
    case 8:   //BS
    case 127: //DEL
      keycode = KEY_BACKSPACE;
      break;
    case 9:   //HT
      keycode = KEY_TAB;
      break;
    case 27:
      in_ascii = HWSERIAL.peek();
      if(in_ascii == 255) {
        keycode = KEY_ESC;
      } else {
        HWSERIAL.read();
        keycode = escape_sequence_to_keycode(in_ascii);
      }
      break;
  }
  return keycode;
}

uint16_t keyname_to_keycode(const char* keyname) {
  uint16_t keycode = 0;

#ifdef MYDEBUG
  SerialPrintfln("\tKeyname: %-15s -> %x", keyname, str2int(keyname));
#endif
  switch (str2int(keyname)) {
    case str2int("Enter"):
      keycode = KEY_ENTER;
      break;
    case str2int("Escape"):
    case str2int("Esc"):
      keycode = KEY_ESC;
      break;
    case str2int("Space"):
      keycode = KEY_SPACE;
      break;
    case str2int("Tab"):
      keycode = KEY_TAB;
      break;
    case str2int("Backspace"):
    case str2int("BS"):
      keycode = KEY_BACKSPACE;
      break;
    case str2int("Delete"):
    case str2int("Del"):
      keycode = KEY_DELETE;
      break;
    case str2int("Insert"):
    case str2int("Ins"):
      keycode = KEY_INSERT;
      break;
    case str2int("Up"):
      keycode = KEY_UP;
      break;
    case str2int("Down"):
      keycode = KEY_DOWN;
      break;
    case str2int("Left"):
      keycode = KEY_LEFT;
      break;
    case str2int("Right"):
      keycode = KEY_RIGHT;
      break;
    case str2int("Home"):
      keycode = KEY_HOME;
      break;
    case str2int("End"):
      keycode = KEY_END;
      break;
    case str2int("PgUp"):
      keycode = KEY_PAGE_UP;
      break;
    case str2int("PgDn"):
      keycode = KEY_PAGE_DOWN;
      break;
  }
  return keycode;
}

uint16_t parse_keyname(const char* keyname) {
  //Returns either the keycode or a ascii char
  uint16_t key = 0;
  key = keyname_to_keycode(keyname);
    SerialPrintfln("Ascii: %s", keyname);
  if (!key && !*(keyname+1))
    key = *keyname;
  return key;
}

void usb_send_key(uint16_t key, uint16_t mod=0) {
  // Uses Keyboard.press from the Teensyduino Core Library
  // key can be either a ascii char or a keycode
#ifdef MYDEBUG
  char key_b[33];
  itoa(key, key_b, 2);
  char mod_b[33];
  itoa(mod, mod_b, 2);
  SerialPrintfln("\tSendKey: %6i = %08s | mod: %6i = %08s", key, key_b, mod, mod_b);
#endif
  if (mod) Keyboard.press(mod);
  Keyboard.press(key);
  Keyboard.releaseAll();
}


// Interactive mode functions
void interactive_mode(char in_ascii) {
  uint16_t keycode;

  if ((keycode = special_char_to_keycode(in_ascii))) {
    usb_send_key(keycode);
    switch(keycode) {
      case KEY_ENTER:
        SerialPrintfln("");
        break;
      case KEY_BACKSPACE:
        SerialDeleteChars(1);
        break;
    }
  } else if (in_ascii <= 26) {
    in_ascii = in_ascii + 'a' - 1;
    usb_send_key(in_ascii, MODIFIERKEY_CTRL);
  } else {
    HWSERIAL.write(in_ascii);
    usb_send_key(in_ascii);
  }
}

// Command mode functions
void command_mode(char in_ascii) {
  uint16_t keycode = special_char_to_keycode(in_ascii);

  if(keycode) {
    switch(keycode) {
      case KEY_ENTER:
        SerialPrintfln("");
        kbd_buff[kbd_idx] = '\0';
        c_parse(kbd_buff);
        crs_idx = kbd_idx;
        kbd_idx = 0;
        break;
      case KEY_BACKSPACE:
        if (crs_idx == kbd_idx) {
          // TODO: Currently only supports deleting characters at the end of the line
          // To delete in the middle of the Array, all following characters must be moved
          SerialDeleteChars(1);
          if (kbd_idx>0) kbd_idx--;
          if (crs_idx>0) crs_idx--;
        }
        break;
      case KEY_LEFT:
        if (crs_idx>0) {
          SerialAnsiEsc("1D");
          crs_idx--;
        }
        break;
      case KEY_RIGHT:
        if (crs_idx<kbd_idx) {
          SerialAnsiEsc("1C");
          crs_idx++;
        }
        break;
      case KEY_UP:
        while (crs_idx>kbd_idx) {
          if (!kbd_buff[kbd_idx])
            kbd_buff[kbd_idx] = ' ';
          HWSERIAL.write(kbd_buff[kbd_idx++]);
        }
        break;
    }
  } else if (in_ascii == 3) {
    SerialClearLine();
    crs_idx = kbd_idx;
    kbd_idx = 0;
  } else if (kbd_idx >= KBD_BUFFSZ-1) {
    command_mode('\n');
  } else if (in_ascii <= 26) {
  } else {
    HWSERIAL.write(in_ascii);
    if (crs_idx>kbd_idx) crs_idx = kbd_idx;
    kbd_buff[crs_idx++] = in_ascii;
    if (kbd_idx<crs_idx) kbd_idx++;
  }
}

void c_parse(char* str) {
  char* pch;

  pch = strtok(str," ");
#ifdef MYDEBUG
  SerialPrintfln("\tCommand: %-15s -> %x", pch, str2int(pch));
#endif
  switch (str2int(pch)) {
    case str2int("SendRaw"):
      // Send the rest of the line literally
      if ((pch = strtok (NULL,"")))
        c_sendraw(pch);
      break;

    case str2int("Send"):
      // Send the rest of the line (and parse special characters)
      if ((pch = strtok (NULL,"")))
        c_send(pch);
      break;

    case str2int("UnicodeLinux"):
    case str2int("UCL"):
      // Send a single unicode character (on Linux)
      if ((pch = strtok (NULL,"")))
        c_unicode(pch, true);
      break;

    case str2int("UnicodeWindows"):
    case str2int("UCW"):
      // Send a single unicode character (on Windows)
      if ((pch = strtok (NULL,"")))
        c_unicode(pch, false);
      break;

    case str2int("Sleep"):
      // Sleep a certain amount of time in ms
      if ((pch = strtok (NULL,"")))
        c_sleep(pch);
      break;

    case str2int("Help"):
      // Display a informative help message
      //TODO
      break;

    default:
      // Check if input is a keyname
      uint16_t key;
      if ((key = keyname_to_keycode(pch))) {
        usb_send_key(key);
      } else {
        // Show warning about invalid command
        //TODO
      }
      break;
  }
}

void c_sendraw(char* pch) {
  char* c = pch;

  while (*c) {
    usb_send_key(*c);
    c++;
  }
}

void c_send(char* pch) {
  char* c = pch;
  int modifier = 0;
  bool braces = false;
  char keyname_buff[KEYNAME_BUFFSZ];
  int keyname_idx = 0;
  uint16_t keycode;

  while (*c) {
    if (braces) {
      if ((keyname_idx >= KEYNAME_BUFFSZ-1) || *c == '}') {
        keyname_buff[keyname_idx] = '\0';
        if ((keycode = parse_keyname(keyname_buff))) {
          usb_send_key(keycode);
        }
        keyname_idx = 0;
        braces = false;
      } else {
        keyname_buff[keyname_idx++] = *c;
      }
      c++;
      continue;
    }
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
      case '{':
        braces = true;
        break;
      default:
        usb_send_key(*c, modifier);
        modifier = 0;
        break;
    }
    c++;
  }
}

void c_unicode(char* pch, bool linux) {
  //XXX
}

void c_sleep(char* pch) {
  int time = atoi(pch);
#ifdef MYDEBUG
  SerialPrintfln("\tSleep %i ms", time);
#endif
  //XXX
}
