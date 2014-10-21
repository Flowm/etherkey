#define HWSERIAL Serial1

#define KBD_BUFFSZ 200
#define PRNT_BUFFSZ 80

char inChar;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
char prnt_buff[PRNT_BUFFSZ];

void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    if (inChar == 3) { //CTRL-C Clear
      HWSERIAL.println("Ctrl-C received, clearing buffer");
      kbd_idx=0;
    } else if (inChar == 16) { //CTRL-P Print
      HWSERIAL.println("Ctrl-P recived, printing buffer in 3 seconds");
      delay(3000);
      for(int i=0; i<kbd_idx; i++) {
        Keyboard.write(kbd_buff[i]);
      }
      kbd_idx = 0;
    } else if (kbd_idx<100) {
      snprintf(prnt_buff, PRNT_BUFFSZ, "Recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
      HWSERIAL.println(prnt_buff);

      kbd_buff[kbd_idx++] = inChar;
    }
  }
}
