#define CLI_CMD_NUM_MAX 6 // Max size of command list
#define CLI_MODE_WAITING 0
#define CLI_MODE_READING 1



class MyCli {
  public:
    MyCli();
    void reset();
    void think();
    void execLine();
    void printHelp();
    void addCmd(MyCliCmd* cmd);
    void addCmd(char *cmd, void (*callback)(char*, MyCliBuffer&));
    byte cliMode;
  private:
    MyCliBuffer buffer;
    uint8_t cmdIdx;
    MyCliCmd* cmdList[CLI_CMD_NUM_MAX];
    byte cmdListIdx;
};

MyCli::MyCli() {
  cliMode = CLI_MODE_WAITING;
  buffer = MyCliBuffer();
  cmdIdx = 0;
  memset(this->cmdList, 0, sizeof(this->cmdList));
  cmdListIdx = 0;
}

void MyCli::printHelp() {
  Serial.println(F("--- HELP ---"));
  for (byte iCmd=0; iCmd < cmdListIdx; iCmd++) {
    /*Serial.print(iCmd); Serial.print(CHAR_SPACE);*/
    /*Serial.print((int)this->cmdList[iCmd], HEX); Serial.print(CHAR_SPACE);*/
    Serial.println(this->cmdList[iCmd]->cmd);
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


void MyCli::addCmd(MyCliCmd* cmd) {
  // Don't add a CMD if the list is full
  if (cmdListIdx >= CLI_CMD_NUM_MAX) {
    Serial.println("ERR CMD LIST FULL");
    return;
  }

  Serial.print("ADD CMD "); Serial.print(cmdListIdx); Serial.print(CHAR_SPACE); Serial.println(cmd->cmd);
  cmdList[cmdListIdx] = cmd;
  cmdListIdx++;
}

void MyCli::think() {
  while (Serial.available()) {
    char c = Serial.read();
    //Serial.print(">"); Serial.println(c);

    // CLI is waiting for an actual character, not a space, newline, or carriage-return
    // On the ASCII chart, printable characters start at number 32
    // https://www.ascii-code.com/
    if (this->cliMode == CLI_MODE_WAITING) {
      if (c >= 32 && c != ' ') {
        this->cliMode = CLI_MODE_READING; // Start reading
        //Serial.println("READING");
      }
    }

    // CLI is reading
    if (this->cliMode == CLI_MODE_READING) {
      if (c == '\n' || c == '\r') {
        // Is an end of line. Execute the line.
        this->execLine();
        return;
      }

      // Add the character to the buffer
      if (!this->buffer.isFull()) {
        this->buffer.add(c);

        // If the buffer is full, execute now
        if (this->buffer.isFull()) {
          this->execLine();
          return;
        }
      }
    }
  }
}

// Execute the CLI command
void MyCli::execLine() {
  Serial.print("EXEC> "); Serial.println(this->buffer.cliBuffer);

  if (this->buffer.cliIdx == 0) {
    // Line buffer is empty, nothing to do here
    Serial.println("EMPTY");
    return; 
  }

  // Get the first part, ready to the first space character
  char *cliPart = this->buffer.startRead(" ");
  strupr(cliPart); // Make uppercase, so command is not case sensitive. "set" and "SeT" is the same as "SET".

  // TODO run commands
  // Loop through commands, looking for one with a matching command.
  // Once found, run it
  bool ranCmd = false;
  if (!strcmp("HELP", cliPart)) {
    this->printHelp();
    ranCmd = true;
  }

  if (!ranCmd) {
    for (byte iCmd=0; iCmd < this->cmdListIdx; iCmd++) {
      //Serial.print("T ");Serial.print(iCmd); Serial.println(this->cmdList[iCmd]->cmd);
      if (!strcmp(this->cmdList[iCmd]->cmd, cliPart)) {
        // Found a match. Run it.
        //Serial.print("Running cmd: "); Serial.println(cliPart);
        this->cmdList[iCmd]->callback(cliPart, this->buffer);
        ranCmd = true;
      }
    }
  }
  
  if (!ranCmd) {
    Serial.println("UNK CMD");
  }
  
  // Cleanup to get ready for the next line
  this->reset();
}

void MyCli::reset() {
  this->cliMode = CLI_MODE_WAITING;
  this->buffer.reset();
}
