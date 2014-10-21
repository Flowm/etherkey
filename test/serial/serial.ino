#define HWSERIAL Serial1

#define PRNT_BUFFSZ 80

char inChar;
char prnt_buff[PRNT_BUFFSZ];

void setup() {
  Serial.begin(115200);
  HWSERIAL.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    inChar = Serial.read();
    snprintf(prnt_buff, PRNT_BUFFSZ, "USB  recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
    Serial.println(prnt_buff);
    HWSERIAL.println(prnt_buff);
  }
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();
    snprintf(prnt_buff, PRNT_BUFFSZ, "UART recv -> Keycode: %i\tCharacter: %c", inChar, inChar);
    Serial.println(prnt_buff);
    HWSERIAL.println(prnt_buff);
  }
}
