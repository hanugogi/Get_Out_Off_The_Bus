#include <SoftwareSerial.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532Interface.h>
#include <PN532.h>

#define NUM_BUS_STOP 4
#define ALARM_SIGNAL '1'
#define BTRX 8
#define BTTX 9
#define BUZZER 6

SoftwareSerial BTSerial(BTRX, BTTX);  
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

String id;
int desti, nowStop;
int Flag[NUM_BUS_STOP] = {0, 0, 0, 0};
bool flag = true, BTflag = false;

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
  
  pinMode(BUZZER, OUTPUT);
  buzzerNFC();
  delay(1000);
  buzzerFail();
  delay(1000);
  buzzerSecess();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    buzzerFail();
    while (1);
  }
  nfc.setPassiveActivationRetries(0x8F);
  nfc.SAMConfig();
}

void loop() {
  if(flag)
    check();
  else {
    checkStop();
    flagCheck();
  }
}

void check()
{
  bool success = false;
  success = nfc.inListPassiveTarget();

  if (success) {
    buzzerNFC();
    
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
    if (success) {
      desti = (int)response[0];
      desti -= '0';
      switch (desti) {
        case 1: Flag[0]++; break;
        case 2: Flag[1]++; break;
        case 3: Flag[2]++; break;
        case 4: Flag[3]++; break;
      }
      flag = false;
    }
    else
      buzzerFail();
  }
  else
    Serial.println("fuxx");
  delay(1000);
}

void checkStop(void) {
  while (Serial.available()==0){
    if(BTSerial.available()>0)
      BTflag = true;
  }
  nowStop = Serial.read()-'0';
}

void flagCheck(void) {
    if (Flag[nowStop+1]>0){
      if(BTflag){
        BTSerial.write(ALARM_SIGNAL);
        delay(2500);
        buzzerSecess();
        Flag[nowStop] = 0;
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

void buzzerFail(void) {
  tone(BUZZER, 2500);
  delay(80);
  noTone(BUZZER);
}

void buzzerSecess(void){
  tone(BUZZER, 523);
  delay(1000);
  noTone(BUZZER);
}
