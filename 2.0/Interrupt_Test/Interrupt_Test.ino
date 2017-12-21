#define interruptPin 2

void checkNFC(void);
void updateFlag(void);

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(interruptPin), checkNFC, FALLING);
  pinMode(interruptPin,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(5);
}


void checkNFC(void) {
  Serial.print("checkNFC");
  while(reading())
  delay(1000);
}

boolean reading(void){
 detachInterrupt(digitalPinToInterrupt(interruptPin));
 Serial.println("reading");
 attachInterrupt(digitalPinToInterrupt(interruptPin), checkNFC, FALLING);
 return 0;
 }
