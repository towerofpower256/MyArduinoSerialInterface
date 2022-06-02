# MyArduinoSerialInterface
Here are my attempts at a serial interface for a microcontroller. I've always wanted to create something like this so I can configure and change projects without needing to re-flash them.

## Version 1
A very simple CLI interface. If the character `b` is sent over the serial connection, it will toggle the LED.
The CLI is blocking, and nothing else is able to run on the controller except for the CLI.

## Version 2
Another simple CLI similar to version one, but this time the character `0` will turn off the LED, and the character `1` will turn it on.
The CLI is still blocking, and doesn't allow anything else to run on the controller except for the CLI.

## Version 3
Same as before, sending 0 and 1 characters over serial, but is now non-blocking and allows the controller to work on other activities alongside the serial interface.

## Version 4
Now uses commands instead of just single characters. Each command is on a single line.

OK = prints the message `I am OK`.
SET = set the value of the variable to something (e.g. `set abc123`).
GET = prints the value of the variable e.g. `That value: abc123`.
Unknown commands get the mesage `Unknown command`.

## Version 5
A relatively complete serial interface, passing CLI details between functions, each command is its own function instead of everything bunched into a single function.

Commands control the onboard LED.
OK = prints the message `I am OK`.
SET = Sets the LED. `SET ON` turns it on, `SET OFF` turns it off.

## Version 6
The big one.

The CLI interface is built out into a object oriented library, and broken out into separate files instead of one massive file.

Commands are defined on startup. Each command is a `MyCliCmd` object, and uses callback functions to handle the incoming command. The command buffer is a `MyCliBuffer` object with helper functions for reading the buffer.

```cpp
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
```

Here's an example CLI command for setting the onboard LED.

```cpp
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
```