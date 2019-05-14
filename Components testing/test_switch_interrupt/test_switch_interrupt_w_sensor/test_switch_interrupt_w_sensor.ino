/*
  Example of a programme for toggling a variable from an external interrupt whose source behaves like a SPST (single pole, single throw) switch.
  Testing was actually done with a SPDT switch, (single pole, double throw), which can behave like a SPST by leaving one throw floating.
*/

#define INTPIN 2
#define LEDPIN 8
#define TRIGPIN 9
#define ECHOPIN 10

long duration; // Duration of the forward-bounce-backward signal
int distance = 1000; // Distance (in cm) of the object and the sensor
int mode; // 0: inside (<20cm), 1: outside (<100cm)
int initialState;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT); // Init the LED_BUILTIN as an output
  // Setting up the pin 7 to be an output for the LED (to be turned on and off in the ISR)
  pinMode(LEDPIN, OUTPUT);
  pinMode(INTPIN, INPUT_PULLUP); //using the pullup like a pro
  pinMode(TRIGPIN,OUTPUT); // Trigger pin as an output
  pinMode(ECHOPIN,INPUT); // Echo pin as an input
  // To know what the starting state is, we read from the INTPIN first
  initialState = digitalRead(INTPIN);
  mode = (initialState == HIGH ? 1 : 0);
  changeMode(mode);
  
  //Setting up the ISR to trigger when a change happens in pin 2.
  attachInterrupt(digitalPinToInterrupt(INTPIN), toggleMode, CHANGE);
  
}

void loop() {
    // Clearing trigPin
    digitalWrite(TRIGPIN,LOW);
    delayMicroseconds(10);
  
    // Sending a pulse for 10 µs
    digitalWrite(TRIGPIN,HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGPIN,LOW);

    // Reading the echo pin for the return of the pulse
    duration = pulseIn(ECHOPIN, HIGH);
  
    // Calculating the distance
    distance = duration*0.034/2;
    

    if(mode){
      if(distance<100){
        digitalWrite(LED_BUILTIN,HIGH); // Turn the LED on if the obstacle is < 100cm away
      }
      else{
        digitalWrite(LED_BUILTIN,LOW); // Turn the LED off if the obstacle is > 100cm away
      }
      
    }
    else{
      if(distance<20){
        digitalWrite(LED_BUILTIN,HIGH); // Turn the LED on if the obstacle is < 20cm away
      }
      else{
        digitalWrite(LED_BUILTIN,LOW); // Turn the LED off if the obstacle is > 20cm away
      }
    }

}

void toggleMode() {
  // toggle the mode and act accordingly
  mode = !mode;
  changeMode(mode);
}

void changeMode(int mode) {
   if (mode) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }
}
