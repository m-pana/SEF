const int outputDigitalPin = 5;
const int analogInput = A0;
int val = 0;

void setup() {
  // Set digital pin to output and make it HIGH (should output a voltage of 5V)
  pinMode(outputDigitalPin, OUTPUT);
  digitalWrite(outputDigitalPin, HIGH);
  Serial.begin(9600);
}

void loop() {
  // Read from A0 every 2 seconds and print it on serial monitor
  delay(2000);
  val = analogRead(analogInput);
  Serial.println(val);
}
