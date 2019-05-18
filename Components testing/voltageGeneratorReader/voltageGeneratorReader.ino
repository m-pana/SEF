int valueRead;
int millivolts;
char out[50];

void setup() {
  // Power generator + reader
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  valueRead = analogRead(A0);
  millivolts = (int) (valueRead * (5000.0/1023.0));
  sprintf(out, "Generating %d millivolts", millivolts);
  Serial.println(out);
  delay(1000);
}
