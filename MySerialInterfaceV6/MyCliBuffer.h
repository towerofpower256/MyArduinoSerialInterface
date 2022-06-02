#define CLI_BUFFER_SIZE 32 // Max length of the serial command buffer

class MyCliBuffer {
  public:
    MyCliBuffer();
    void add(char c);
    void reset();
    bool isFull();
    bool isEmpty();
    char *startRead(char* delim);
    char *readTo(char* delim);
    char cliBuffer[CLI_BUFFER_SIZE];
    byte cliIdx;
  private:
    char *cliPartPtr;
};

MyCliBuffer::MyCliBuffer() {
  this->reset();
}

bool MyCliBuffer::isFull() {
  return (cliIdx >= CLI_BUFFER_SIZE-1);
}

void MyCliBuffer::add(char c) {
  Serial.print(cliIdx); Serial.print(CHAR_SPACE); Serial.println(c);
  
  cliBuffer[cliIdx] = c;
  cliIdx++;
}

void MyCliBuffer::reset() {
  cliIdx = 0;
  memset(cliBuffer, 0, CLI_BUFFER_SIZE);
}

char *MyCliBuffer::startRead(char* delim) {
  return strtok_r(cliBuffer, delim, &cliPartPtr);
}

char *MyCliBuffer::readTo(char* delim) {
  return strtok_r(NULL, delim, &cliPartPtr);
}
