#include <SoftwareSerial.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532Interface.h>
#include <PN532.h>

#define NUM_BUS_STOP 4
#define ALARM_SIGNAL ('5')
#define BTRX 8
#define BTTX 9
#define DTRX 4
#define DTTX 5
#define BUZZER 6

SoftwareSerial BTSerial(BTRX, BTTX);
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

int desti = 3; // 목적지 코드
String id;
int Flag[NUM_BUS_STOP] = {0}; // Flag 첫 정류장은
int i, j; //반복문을 위한 변수
bool success = false;
bool breakpoint = false; // breakpoint (checkStop());
char nowStop; // 현재 버스 위치 코드를 받기 위한 변수
bool flag = true;
bool BTflag = false;

uint8_t response[30];
uint8_t responseLength = 30;
uint8_t selectApdu[] = { 0x00, /* CLA */
                         0xA4, /* INS */
                         0x04, /* P1  */
                         0x00, /* P2  */
                         0x07, /* Length of AID  */
                         (byte)0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                         0x00  /* Le  */
                       };

void check(void);

void checkStop(void) {
  //Serial.println("CheckStop");
  while (Serial.available()==0){
    //Serial.println("waiting");
   // Serial.println(direct.available());
    //Serial.println(direct.read());
    delay(100);
  }
    nowStop = Serial.read();
    nowStop--;
    //Serial.print("nowStop : ");
    //Serial.println(nowStop);
}

void flagCheck(void) {
  if(BTflag){
    if (Flag[nowStop] > 0){ // 0 1 2 일 경우 앞 정류장에 사용자가 내리려고 한다면, 알람을 울리는 신호를 보내고 벨을 울린다.
      //Serial.println("flagCheck");
      BTSerial.write(49); // 폰으로 한 정류장 전에 알람을 전송
      delay(2500);
      ringBell();
    }
  }
}

void buzzerNFC(void) {
  tone(BUZZER, 4500);
  delay(200);
  tone(BUZZER, 5500);
  delay(150);
  noTone(BUZZER);
}

void ringBell(void) {
  tone(BUZZER, 5500);
  delay(400);
  noTone(BUZZER);
}

void buzzerFail(void) {
  tone(BUZZER, 2500);
  delay(80);
  noTone(BUZZER);
}
