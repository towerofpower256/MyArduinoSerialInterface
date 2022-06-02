// MySerialInterface v2
// Turn the LED on and off by sending 0 and 1 over serial
// By David McDonald 17/05/2022

#define LEDPIN LED_BUILTIN

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("START");
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '0') {
      Serial.println("OFF");
      digitalWrite(LEDPIN, LOW);
    }
    if (c == '1') {
      Serial.println("ON");
      digitalWrite(LEDPIN, HIGH);
    }
  }
}
