// MySerialInterface v4
// Now uses commands instead of just a single character.
// GET and SET a value into a variable.
// By David McDonald 17/05/2022

#define LEDPIN LED_BUILTIN
#define MILLIS_INTERVAL 1000

#define CLI_BUFFER_SIZE 32 // Max length of the serial command buffer
#define CLI_MODE_WAITING 0
#define CLI_MODE_READING 1

unsigned long lastMillis = 0;

char cliBuffer[CLI_BUFFER_SIZE];
byte cliMode;
byte cliIdx;

char thatValue[CLI_BUFFER_SIZE];

void doMillis() {
  if ((millis() - lastMillis) < MILLIS_INTERVAL) return;
  lastMillis = millis();
  Serial.println(millis());
}

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

  char delim[] = " "; // Delimiters, should just be space characters
  char eolDelim[] = "\r\n";
  char *part;
  char *saveptr;
  
  // Get the first part
  part = strtok_r(cliBuffer, delim, &saveptr);
  strupr(part); // Make uppercase, so command is not case sensitive. "set" and "SeT" is the same as "SET".

  // Take action, depending on what the command was
  // strcmp compares 2 char arrays. If they are the same, the result will be zero.
  if (!strcmp(part,"OK")) {
    Serial.println("I am OK");
    
  } else if (!strcmp(part,"SET")) {
    // Get the next part.
    // Use NULL to resume from where you started in the line buffer string
    // This time, don't stop until the end of the line using eolDelim
    part = strtok_r(NULL, eolDelim, &saveptr);

    // Save that value
    setThatValue(part); 
    
  } else if (!strcmp(part,"GET")) {
    Serial.print("That value: '"); Serial.print(getThatValue()); Serial.println('\'');
    
  } else {
    Serial.println("Unknown command");
  }

  // Cleanup to get ready for the next line
  cliReset();
}

void setThatValue(char* newValue) {
  strcpy(thatValue, newValue);
}

char* getThatValue() {
  return thatValue;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("START");
  cliReset();
}

void loop() {
  // put your main code here, to run repeatedly:
  doCli();
  doMillis();
}
