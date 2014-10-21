#define HWSERIAL Serial1

#define PRNT_BUFFSZ 80

char inChar;
char prnt_buff[PRNT_BUFFSZ];

void setup() {
  HWSERIAL.begin(115200);
  delay(1000);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();

    snprintf(prnt_buff, PRNT_BUFFSZ, "Recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
    HWSERIAL.println(prnt_buff);

    Keyboard.write(inChar);
  }
}
