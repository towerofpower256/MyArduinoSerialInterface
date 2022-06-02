// MySerialInterface v1
// Using single characters from a serial interface to
// turn the LED on and off.
// By David McDonald 17/05/2022

#define LEDPIN LED_BUILTIN

bool ledOn = false;

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
    if (c == 'b') {
      toggleLed();
    }
  }
}

void toggleLed() {
  ledOn = !ledOn;
  digitalWrite(LEDPIN, ledOn);
  if (ledOn) Serial.println("ON"); else Serial.println("OFF");
}
