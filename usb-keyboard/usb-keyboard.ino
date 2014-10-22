#define HWSERIAL Serial1

#define KBD_BUFFSZ 200
#define PRNT_BUFFSZ 80

char inChar;
char kbd_buff[KBD_BUFFSZ];
char prnt_buff[PRNT_BUFFSZ];
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
      snprintf(prnt_buff, PRNT_BUFFSZ, "Switching to mode %i", in_mode);
      HWSERIAL.println(prnt_buff);
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
        snprintf(prnt_buff, PRNT_BUFFSZ, "Recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
        HWSERIAL.println(prnt_buff);
        break;
    }
  }
}

void parse(char * str) {
}
