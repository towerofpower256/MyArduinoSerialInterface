#define CLI_CMD_NUM_MAX 6 // Max size of command list
#define CLI_BUFFER_SIZE 32 // Max length of the serial command buffer

#define CLI_MODE_WAITING_START 0
#define CLI_MODE_READING 1
#define CLI_MODE_WAITING_EOL 2

char MSG_OVERSIZE_LINE[] = "Line too long";
char MSG_UNKNOWN_CMD[] = "UNK CMD";

class MyCli {
  public:
    MyCli();
    void reset();
    void think();
    void execLine();
    void printHelp();
    void addCmd(char *cmd, void (*callback)(char*, char*));
    byte cliMode;
  private:
    char buffer[CLI_BUFFER_SIZE];
    byte bufferIdx;
    void bufferReset();
    
    //MyCliCmd* cmdList[CLI_CMD_NUM_MAX];
    byte cmdListIdx;
    char* cmdListCmd[CLI_CMD_NUM_MAX];
    void (*cmdListCallback[CLI_CMD_NUM_MAX])(char*, char*);
};

MyCli::MyCli() {
  cliMode = CLI_MODE_WAITING_START;
  memset(buffer, 0, CLI_BUFFER_SIZE);
  
  cmdListIdx = 0;
  memset(cmdListCmd, 0, sizeof(cmdListCmd));
  memset(cmdListCallback, 0, sizeof(cmdListCallback));

  bufferReset();
}

void MyCli::printHelp() {
  Serial.println(F("--- HELP ---"));
  for (byte iCmd=0; iCmd < cmdListIdx; iCmd++) {
    Serial.println(this->cmdListCmd[iCmd]);
  }
}

// DOESN'T WORK
// Because this '_cmd' reference dies with this function call, the object is garbage collected,
// even through we're storing a reference to it.
// Then, when we run this to make another CMD object, the new one takes the same place in
// memory as the old one. The end result is a big list of the same objects, a list of CMD
// pointers, all pointing to the same place in memory.
// Yes, I could get around this by using 'malloc' to forcefully allocate the space in memory,
// but I don't want to open that pandora's box, and risk a memory leak.
// E.g.
// [0] 0x08CA MILLIS
// [1] 0x08CA MILLIS
// [2] 0x08CA MILLIS
/*
void MyCli::addCmd(char *cmd, void (*callback)(char*, MyCliBuffer&)) {
  MyCliCmd* _cmd = (MyCliCmd*)malloc(sizeof(MyCliCmd));
  _cmd = &MyCliCmd(cmd, callback);
  this->addCmd(_cmd);
}
*/


void MyCli::addCmd(char* cmd, void (*_callback)(char*, char*)) {
  // Don't add a CMD if the list is full
  if (cmdListIdx >= CLI_CMD_NUM_MAX) {
    Serial.println("ERR CMD LIST FULL");
    return;
  }

  // Make commands upper case, to ignore case sensitivity
  strupr(cmd);

  Serial.print("ADD CMD "); Serial.print(cmdListIdx); Serial.print(CHAR_SPACE); Serial.println(cmd);
  cmdListCmd[cmdListIdx] = cmd;
  cmdListCallback[cmdListIdx] = _callback;
  cmdListIdx++;
}

void MyCli::think() {
  while (Serial.available()) {
    char c = Serial.read();
    //Serial.print(bufferIdx); Serial.print(' '); Serial.println(c);

    // CLI is waiting for an actual character, not a space, newline, or carriage-return
    // On the ASCII chart, printable characters start at number 32
    // https://www.ascii-code.com/
    if (cliMode == CLI_MODE_WAITING_START) {
      if (c >= 32 && c != ' ') {
        cliMode = CLI_MODE_READING; // Start reading
        //Serial.println("SREAD");
      }
    }

    // CLI is waiting for the end of line. Previous line was probably bad.
    if (cliMode == CLI_MODE_WAITING_EOL) {
      if (c == '\n' || c == '\r') {
        cliMode = CLI_MODE_WAITING_START;
        return;
      }
    }

    // CLI is reading
    if (cliMode == CLI_MODE_READING) {
      if (c == '\n' || c == '\r') {
        // Is an end of line. Execute the line.
        //Serial.println("EOL");
        this->execLine();
        return;
      }

      // Add the character to the buffer
      if (bufferIdx < CLI_BUFFER_SIZE) {
        //Serial.println("ADD C");
        buffer[bufferIdx] = c;
        bufferIdx++;

        // If the buffer is full
        if (bufferIdx >= CLI_BUFFER_SIZE-1) {
          // Overflow
          Serial.println(MSG_OVERSIZE_LINE);
          cliMode = CLI_MODE_WAITING_EOL;
          return;
        }
      }
    }
  }
}

// Execute the CLI command
void MyCli::execLine() {
  Serial.print("EXEC> "); Serial.println(buffer);

  if (bufferIdx == 0) {
    // Line buffer is empty, nothing to do here
    Serial.println("EMPTY");
    return; 
  }

  // Get the first part, ready to the first space character
  char delim[] = " \r\n";
  char* cliPartPtr;
  char* cliPart = strtok_r(buffer, delim, &cliPartPtr);
  strupr(cliPart); // Make uppercase, so command is not case sensitive. "set" and "SeT" is the same as "SET".
  //Serial.print("CMD> "); Serial.println(cliPart);

  // Loop through commands, looking for one with a matching command.
  // Once found, run it
  bool ranCmd = false;
  if (!strcmp("HELP", cliPart)) {
    this->printHelp();
    ranCmd = true;
  }

  if (!ranCmd) {
    for (byte iCmd=0; iCmd < cmdListIdx; iCmd++) {
      //Serial.print("T ");Serial.print(iCmd); Serial.println(cmdListCmd[iCmd]);
      if (!strcmp(cmdListCmd[iCmd], cliPart)) {
        // Found a match. Run it.
        //Serial.print("Running cmd: "); Serial.println(cliPart);
        cmdListCallback[iCmd](cliPart, cliPartPtr);
        ranCmd = true;
      }
    }
  }
  
  if (!ranCmd) {
    Serial.println(MSG_UNKNOWN_CMD);
  }
  
  // Cleanup to get ready for the next line
  this->reset();
}

void MyCli::reset() {
  cliMode = CLI_MODE_WAITING_START;
  bufferReset();
}

void MyCli::bufferReset() {
  bufferIdx = 0;
  memset(buffer, 0, CLI_BUFFER_SIZE);
}
