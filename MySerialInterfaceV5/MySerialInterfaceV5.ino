// MySerialInterface v5
// A relatively complete serial interface, using functions to the commands, passing CLI details between scopes.
// GET and SET a value into a variable.
// By David McDonald 20/05/2022

#define LEDPIN LED_BUILTIN

#define CLI_BUFFER_SIZE 32 // Max length of the serial command buffer
#define CLI_MODE_WAITING 0
#define CLI_MODE_READING 1

char cliBuffer[CLI_BUFFER_SIZE];
byte cliMode;
byte cliIdx;
char *cliPartPtr;
char *cliPart;
const char cliDelimSpace[] = " ";
const char cliDelimEol[] = "\r\n";

bool ledValue = false;
const char ledValOn[] = "ON";
const char ledValOff[] = "OFF";

void cliReset() {
  cliMode = CLI_MODE_WAITING;
  cliIdx = 0;
  memset(cliBuffer, '\0', CLI_BUFFER_SIZE);
}

void doCli() {
  while (Serial.available()) {
    char c = Serial.read();

    // CLI is waiting for an actual character, not a space, newline, or carriage-return
    // On the ASCII chart, printable characters start at number 32
    // https://www.ascii-code.com/
    if (cliMode == CLI_MODE_WAITING) {
      if (c >= 32 && c != ' ') {
        cliMode = CLI_MODE_READING; // Start reading
      }
    }

    // CLI is reading
    if (cliMode == CLI_MODE_READING) {
      if (c == '\n' || c == '\r') {
        // Is an end of line. Execute the line.
        cliExec();
        return;
      }

      // Add the character to the buffer
      if (cliIdx < CLI_BUFFER_SIZE-1) {
        cliBuffer[cliIdx] = c;
        cliIdx++;

        // If the buffer is full, execute now
        if (cliIdx >= CLI_BUFFER_SIZE-1) {
          cliExec();
          return;
        }
      }
    }
  }
}

// Execute the CLI command
void cliExec() {
  Serial.print("EXEC> "); Serial.println(cliBuffer);

  if (cliIdx == 0) return; // Line buffer is empty, nothing to do here
  
  // Get the first part
  cliPart = strtok_r(cliBuffer, cliDelimSpace, &cliPartPtr);
  strupr(cliPart); // Make uppercase, so command is not case sensitive. "set" and "SeT" is the same as "SET".

  // Take action, depending on what the command was
  // strcmp compares 2 char arrays. If they are the same, the result will be zero.
  if (!strcmp(cliPart,"OK")) {
    cliOK();
    
  } else if (!strcmp(cliPart,"SET")) {
    cliSetLed();
  } else {
    Serial.print("Unknown command "); Serial.println(cliPart);
  }

  // Cleanup to get ready for the next line
  cliReset();
}

void cliOK() {
  Serial.println("I am OK");
}

void cliSetLed() {
  const char cliDelimSpaceEol[] = " \r\n";
  cliPart = strtok_r(NULL, cliDelimSpaceEol, &cliPartPtr);
  strupr(cliPart);
  
  if (!strcmp(cliPart, ledValOn)) {
    setLed(true);
  } else if (!strcmp(cliPart, ledValOff)) {
    setLed(false);
  }
}

void setLed(bool a) {
  ledValue = a;
  digitalWrite(LEDPIN, a);
  Serial.print("LED is ");
  if (ledValue) {
    Serial.println(ledValOn);
  } else {
    Serial.println(ledValOff);
  }
}

// =====================================
// MAIN
// =====================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("START");
  cliReset();
}

void loop() {
  // put your main code here, to run repeatedly:
  doCli();
}
