#include <LiquidCrystal.h>
#include "LowPower.h" 
 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/** First try on making the ultrasonic sensor work.
 *  Main goal is to make the led go on when an obstacle is detected.
 *  Speed of sound: ~340m/s <==> ~0.034cm/µs
 *  Get the distance (in cm): ECHO_PIN_VALUE * 0.034 / 2 
 *  Divide by 2 because it's half the time to make the forward-bounce-backward
 */

 const int trigPin = 8; // Trigger pin = 8
 const int echoPin = 9; // Echo pin = 9

 long duration; // Duration of the forward-bounce-backward signal
 int distance; // Distance (in cm) of the object and the sensor

/**
 * setup() This function runs at the beginning only once.
 * The initialization function.
 */
void setup() {
  pinMode(LED_BUILTIN,OUTPUT); // Init the LED_BUILTIN as an output

  pinMode(trigPin,OUTPUT); // Trigger pin as an output
  pinMode(echoPin,INPUT); // Echo pin as an input 

  Serial.begin(9600); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2); 
  
}

/**
 * The program's main loop.
 */
void loop() {
  // Clearing trigPin
  digitalWrite(trigPin,LOW);
  delayMicroseconds(10);

  // Sending a pulse for 10 µs
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  // Reading the echo pin for the return of the pulse
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration*0.034/2;
  Serial.println(distance); 

  if(distance<100){
    digitalWrite(LED_BUILTIN,HIGH); // Turn the LED on if the obstacle is < 100cm away
  }
  else{
    digitalWrite(LED_BUILTIN,LOW); // Turn the LED off if the obstacle is > 100cm away
  } 
  lcd.clear();
   // Print a message to the LCD.
  lcd.print(distance); 
   
  LowPower.powerDown(SLEEP_1S,ADC_ON,BOD_ON); 

  
}
