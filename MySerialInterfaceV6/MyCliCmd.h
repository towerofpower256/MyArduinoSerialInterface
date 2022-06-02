class MyCliCmd {
  public:
    MyCliCmd(char *cmd, void (*callback)(char*, MyCliBuffer&));
    char *cmd;
    void (*callback)(char*, MyCliBuffer&);
  private:
    
};

MyCliCmd::MyCliCmd(char* _cmd, void (*_callback)(char*, MyCliBuffer&)) {
  this->cmd = _cmd;
  this->callback = _callback;

  strupr(this->cmd); // Make the CMD uppercase, don't care about case sensitivity;
}
