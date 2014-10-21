#define HWSERIAL Serial1

char inChar;

void setup() {
  Serial.begin(115200);
  HWSERIAL.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    inChar = Serial.read();
    HWSERIAL.write(inChar);
  }
  if (HWSERIAL.available() > 0) {
    inChar = HWSERIAL.read();
    Serial.write(inChar);
  }
}
