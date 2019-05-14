#define intpin 2
#define buzpin 6

int res;
volatile int lol = 33;
char out[20];

void setup() {
  pinMode(intpin, INPUT_PULLUP);
  pinMode(buzpin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(intpin), buzz, CHANGE);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  noInterrupts();
  res = digitalRead(intpin);
  sprintf(out, "%d - %d", res, lol);
  Serial.println(out);
  interrupts();
  delay(1000);
}

void buzz() {
  if (lol == 33)
    lol = 44;
  else lol = 33;
}
