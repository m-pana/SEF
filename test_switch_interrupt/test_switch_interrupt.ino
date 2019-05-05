/*
  Example of a programme for toggling a variable from an external interrupt whose source behaves like a SPST (single pole, single throw) switch.
  Testing was actually done with a SPDT switch, (single pole, double throw), which can behave like a SPST by leaving one throw floating.
*/

#define LEDPIN 8
#define INTPIN 2

int mode;
int initialState;

void setup() {
  // Setting up the pin 7 to be an output for the LED (to be turned on and off in the ISR)
  pinMode(LEDPIN, OUTPUT);
  pinMode(INTPIN, INPUT_PULLUP); //using the pullup like a pro
  // To know what the starting state is, we read from the INTPIN first
  initialState = digitalRead(INTPIN);
  mode = (initialState == HIGH ? 1 : 0);
  changeMode(mode);
  
  //Setting up the ISR to trigger when a change happens in pin 2.
  attachInterrupt(digitalPinToInterrupt(INTPIN), toggleMode, CHANGE);
  
}

void loop() {
    // put your main code here, to run repeatedly:
    // ...or not.
}

void toggleMode() {
  // toggle the mode and act accordingly. This is the ISR, it just changes the value if "mode" to the opposite state and delegates to changeMode
  mode = !mode;
  changeMode(mode);
}

void changeMode(int mode) {
  // Utility function to change the actual state of the pin. It was made a standalone function because it's not only used by the ISR, but also by the setup
   if (mode) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }
}
