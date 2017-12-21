#include <SoftwareSerial.h>

#define STOP1 A0
#define STOP2 A1
#define STOP3 A2
#define STOP4 A3
#define DTRX 12
#define DTTX 13

SoftwareSerial DirectSerial(DTRX,DTTX);

int IR_value[4] = {0,};
int Now = 0, Past = -1;

void setup() {
  //Serial.begin(9600); 
  DirectSerial.begin(9600);
  pinMode(STOP1,INPUT);
  pinMode(STOP2,INPUT);
  pinMode(STOP3,INPUT);
  pinMode(STOP4,INPUT);
  pinMode(3,OUTPUT); //PWM 주파수가 490Hz(3, 9, 10, 11번 핀)으로 고정되어 있음
  pinMode(9,OUTPUT); 
  pinMode(10,OUTPUT);  
  pinMode(11,OUTPUT); 
  pinMode(4,OUTPUT); //LED 제어용
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
}

void loop() {
  uint8_t c;
  IR_value[0] = analogRead(STOP1);
  IR_value[1] = analogRead(STOP2);
  IR_value[2] = analogRead(STOP3);
  IR_value[3] = analogRead(STOP4);
  
  analogWrite(3,IR_value[0]/4);
  analogWrite(9,IR_value[1]/4);
  analogWrite(10,IR_value[2]/4);
  analogWrite(11,IR_value[3]/4);
  
  for(int i=0;i<4;i++){
    if(IR_value[i] <= 500){
      Now = i+1;
      
      if(Now == Past)
       break;
      DirectSerial.write(Now+48);
      digitalWrite(Now+3,HIGH);
      Past = Now;
      delay(800);
      
      digitalWrite(Now+3,LOW);
    }
  } 
}
