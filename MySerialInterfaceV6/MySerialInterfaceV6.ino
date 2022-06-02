// MySerialInterface v6
// A command line library as a self-contained extensible object.
// By David McDonald 27/05/2022

#define CHAR_SPACE " "

#include "MyCliBuffer.h"
#include "MyCliCmd.h"
#include "MyCli.h"

const char CLI_DELIM_SPACE[] = CHAR_SPACE;
const char CLI_DELIM_EOL[] = "\r\n";
const char CLI_DELIM_SPACE_EOL = " \r\n";

// ======================================================
// Main program
// ======================================================

#define LEDPIN LED_BUILTIN

bool ledValue = false;
const char ledValOn[] = "ON";
const char ledValOff[] = "OFF";


void cliOK(char *cmd, MyCliBuffer& buffer) {
  Serial.println("I am OK");
}

void cliSetLed(char *cmd, MyCliBuffer& buffer) {
  char* cliPart = buffer.readTo(CLI_DELIM_SPACE_EOL);
  strupr(cliPart); // Make uppercase, don't care about case sensitivity
  
  if (!strcmp(cliPart, ledValOn)) {
    setLed(true);
  } else if (!strcmp(cliPart, ledValOff)) {
    setLed(false);
  } else {
    Serial.println("UNK LED CMD");
  }
}

void cliMillis(char *cmd, MyCliBuffer& buffer) {
  Serial.println(millis());
}

void setLed(bool a) {
  ledValue = a;
  digitalWrite(LEDPIN, a);
  Serial.print("LED ");
  if (ledValue) {
    Serial.println(ledValOn);
  } else {
    Serial.println(ledValOff);
  }
}

// =====================================
// MAIN
// =====================================

MyCli cli;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("START");

  // Setup the CLI
  cli = MyCli();

  // Explicitly construct the CMD objects, then pass them through.
  MyCliCmd cmdOk = MyCliCmd("ok", &cliOK);
  cli.addCmd(&cmdOk);
  
  MyCliCmd cmdSet = MyCliCmd("set", &cliSetLed);
  cli.addCmd(&cmdSet);

  MyCliCmd cmdMillis = MyCliCmd("millis", &cliMillis);
  cli.addCmd(&cmdMillis);
}

void loop() {
  // put your main code here, to run repeatedly:
  cli.think();
}
