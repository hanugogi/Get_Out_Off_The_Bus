#include <SoftwareSerial.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532Interface.h>
#include <PN532.h>

#define NUM_BUS_STOP 4
#define ALARM_SIGNAL ('5')
#define BTRX 8
#define BTTX 9
#define BUZZER 6

SoftwareSerial BTSerial(BTRX, BTTX);  
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

int desti; // 목적지 코드
String id;
int Flag[NUM_BUS_STOP] = {0, 0, 0, 0}; // Flag 첫 정류장은
int i, j; //반복문을 위한 변수
bool success = false;
bool breakpoint = false; // breakpoint (checkStop());
int nowStop; // 현재 버스 위치 코드를 받기 위한 변수
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

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(9600);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    //Serial.print("Cant Find PN532");
    while (1); // halt
  }
  //Serial.println("Found chip PN532");
  nfc.setPassiveActivationRetries(0x8F);
  nfc.SAMConfig();

  /*BTSerial.begin(9600);*/
  pinMode(BUZZER, OUTPUT);
  tone(BUZZER, 1000);
  delay(100);
  noTone(BUZZER);
}

void loop() {
  if (flag)
    check();
  else {
    checkStop();
    flagCheck();
    if(BTSerial.available()>0) 
      BTflag = true;
  }
}

void check()
{
  //Serial.println("Check");
  bool success;

  uint8_t responseLength = 32;

  //Serial.println("Waiting for an ISO14443A card");

  // set shield to inListPassiveTarget
  success = nfc.inListPassiveTarget();

  if (success) {
    buzzerNFC();
    //Serial.println("Found something!");

    uint8_t selectApdu[] = { 0x00, /* CLA */
                             0xA4, /* INS */
                             0x04, /* P1  */
                             0x00, /* P2  */
                             0x07, /* Length of AID  */
                             (byte)0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                             0x00  /* Le  */
                           };

    uint8_t response[32];

    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);

    if (success) {
      for (uint8_t i = 0; i < responseLength; i++) {
        char c = response[i];
        if (i == 0)
          desti = c - 48;
        else if (i >= 2)
          id += c;
      }
      //Serial.print(desti);
      //Serial.println(id);
      switch (desti) {
        case 1: Flag[0]++; break;
        case 2: Flag[1]++; break;
        case 3: Flag[2]++; break;
        case 4: Flag[3]++; break;
      }
      flag = false;
    }
    else {
      buzzerFail();
      //Serial.println("Failed sending SELECT AID");
    }
  }
  else {
    //Serial.println("Didn't find anything!");
  }

  delay(1000);
}

void checkStop(void) {
  //Serial.println("CheckStop");
  while (Serial.available()==0){if(BTSerial.available()>0) BTflag = true;}
    nowStop = Serial.read()-48;
    //Serial.print("nowStop : ");
    //Serial.println(nowStop);
}

void flagCheck(void) {
    if (Flag[nowStop+1]>0){ // 0 1 2 일 경우 앞 정류장에 사용자가 내리려고 한다면, 알람을 울리는 신호를 보내고 벨을 울린다.
    //Serial.println("flagCheck");
      if(BTflag){
        BTSerial.write(49); // 폰으로 한 정류장 전에 알람을 전송
        delay(2500);
        ringBell();
        Flag[nowStop] = 0; // 다음 정류장의 플래그를 초기화
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
