// MySerialInterface v7
// A command line library as a self-contained extensible object.
// But this time, with commands for setting and getting configuration.
// By David McDonald 20/06/2022

#define CHAR_SPACE " "

#include "MyCli.h"

char CLI_DELIM_SPACE[] = CHAR_SPACE;
char CLI_DELIM_EOL[] = "\r\n";
char CLI_DELIM_SPACE_EOL[] = " \r\n";
char MSG_OK[] = "OK";
char MSG_OVERSIZE_PARM[] = "Parm too long";
char MSG_INVALID_NUMBER[] = "Invalid number";

// ======================================================
// Global and settings
// ======================================================

#define CONFIG_STRING_SIZE 31

char CFG_WTIMEOUT[] = "WTIMEOUT";
char CFG_WSSID[] = "WSSID";
char CFG_WPWD[] = "WPWD";

typedef struct {
  int wifiTimeout;
  char wifiSSID[CONFIG_STRING_SIZE];
  char wifiPassword[CONFIG_STRING_SIZE];
} MySettings;

MySettings mySettings;

void setupSettings() {
  // Reset the settings back to default
  memset(&mySettings, 0, sizeof(MySettings));
  mySettings.wifiTimeout = 60;
}

void cliSetConfig(char* cmd, char* buffer) {
  char* cliPart = strtok(buffer, CLI_DELIM_SPACE);
  strupr(cliPart);
  Serial.print("CONF: "); Serial.println(cliPart);

  if (!strcmp(cliPart, CFG_WSSID)) {
    char strbuffer[CONFIG_STRING_SIZE];
    memset(strbuffer, 0, CONFIG_STRING_SIZE);
    cliPart = strtok(NULL, CLI_DELIM_SPACE_EOL);
    if (strlen(cliPart) < CONFIG_STRING_SIZE) {
      strcpy(mySettings.wifiSSID, cliPart);
      Serial.println(MSG_OK);
    } else {
      Serial.println(MSG_OVERSIZE_PARM);
    }

    return;
  } else if (!strcmp(cliPart, CFG_WPWD)) {
    char strbuffer[CONFIG_STRING_SIZE];
    memset(strbuffer, 0, CONFIG_STRING_SIZE);
    cliPart = strtok(NULL, CLI_DELIM_SPACE_EOL);
    if (strlen(cliPart) < CONFIG_STRING_SIZE) {
      strcpy(mySettings.wifiPassword, cliPart);
      Serial.println(MSG_OK);
    } else {
      Serial.println(MSG_OVERSIZE_PARM);
    }

    return;
  } else if (!strcmp(cliPart, CFG_WTIMEOUT)) {
    cliPart = strtok(NULL, CLI_DELIM_SPACE_EOL);
    if (cliPart == '0') { 
      mySettings.wifiTimeout = 0;
      Serial.println(MSG_OK);
      return;
    }
    
    int intBuffer = atoi(cliPart);
    if (intBuffer == 0) {
      Serial.println(MSG_INVALID_NUMBER);
    } else {
      mySettings.wifiTimeout = intBuffer;
      Serial.println(MSG_OK);
    }
    return;
  } else {
    Serial.println(F("Unknown config"));
    return;
  }
}

void cliGetConfig(char* cmd, char* buffer) {
  Serial.println(F("--- CONFIG ---"));
  Serial.print(CFG_WTIMEOUT); Serial.print('\t'); Serial.println(mySettings.wifiTimeout);
  Serial.print(CFG_WSSID); Serial.print('\t'); Serial.println(mySettings.wifiSSID);
  Serial.print(CFG_WPWD); Serial.print('\t'); Serial.println(mySettings.wifiPassword);
}

// ======================================================
// Main program
// ======================================================

#define LEDPIN LED_BUILTIN

bool ledValue = false;
const char ledValOn[] = "ON";
const char ledValOff[] = "OFF";


void cliOK(char *cmd, char* buffer) {
  Serial.println("I am OK");
}

void cliSetLed(char *cmd, char* buffer) {
  char* cliPart = strtok(buffer, CLI_DELIM_SPACE_EOL);
  strupr(cliPart); // Make uppercase, don't care about case sensitivity
  //Serial.print("LED part "); Serial.println(cliPart);
  
  if (!strcmp(cliPart, ledValOn)) {
    setLed(true);
  } else if (!strcmp(cliPart, ledValOff)) {
    setLed(false);
  } else {
    Serial.println("UNK LED CMD");
  }
}

void cliMillis(char *cmd, char* buffer) {
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

  // Setup settings
  setupSettings();

  // Setup the CLI
  cli = MyCli();

  cli.addCmd("ok", &cliOK);
  cli.addCmd("led", &cliSetLed);
  cli.addCmd("millis", &cliMillis);
  cli.addCmd("set", &cliSetConfig);
  cli.addCmd("get", &cliGetConfig);
}

void loop() {
  // put your main code here, to run repeatedly:
  cli.think();
}
