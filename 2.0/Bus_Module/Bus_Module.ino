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
#define MAX 10  //총 User 수 - 미사용

SoftwareSerial BTSerial(BTRX, BTTX);
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

char desti; // 목적지 코드
int Flag[NUM_BUS_STOP] = {0}; // Flag 첫 정류장은 
int i, j; //반복문을 위한 변수
bool success = false;
bool breakpoint = false; // breakpoint (checkStop());
char nowStop; // 현재 버스 위치 코드를 받기 위한 변수

int count = 0;
int flag = 0;

uint8_t response[30];
uint8_t responseLength = 30;
uint8_t selectApdu[] = { 0x00, /* CLA */
                         0xA4, /* INS */
                         0x04, /* P1  */
                         0x00, /* P2  */
                         0x07, /* Length of AID  */
                         (byte)0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                         0x00  /* Le  */ };

typedef struct User{
  int desti;
  String id;
};//유저 data 담는 구조체 - 미사용 
User *user;

void ringAlarm(void) { BTSerial.write(ALARM_SIGNAL); } // 알람을 보낸다
void ringBell(void); // 벨을 키라는 신호를 준영이단에서 받고 벨을 킨다
int updateFlag(void); //현재 Desti를 체크하고 Flag를 업데이트 한다
void checkStop(void); // 현재 버스 위치를 받아온다. From 준영
int checkNFC(void);

void setup()
{   
    Serial.begin(9600);
    Serial.println("-------Peer to Peer HCE--------");
    nfc.begin();
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      Serial.println("Can't find PN532");
      while (1);
    }
    Serial.println("Found chip PN532");
    nfc.setPassiveActivationRetries(0x8F);
    //nfc.SAMConfig();
    
    BTSerial.begin(9600);
    pinMode(BUZZER, OUTPUT);
    tone(BUZZER, 1000);
    noTone(BUZZER);
    User users[MAX];
    user = &users[0];

    delay(12);
    
    //nfc.inListPassiveTarget();
    //attachInterrupt(digitalPinToInterrupt(PN532_IRQ), updateFlag(), CHANGE);
    
    /*while(!flag);//데이터를 읽어올 타겟이 있는지 확인
      Serial.println(flag);*/
    //if(success) checkStop(); // 첫번째 실행에 한하여 NFC를 받는 순간 시퀀스가 시작됨

}

void loop()
{   
    /*checkStop(); //준영이 라인에서 현재 버스의 위치를 받아온다. 받아오는데 실패할 경우 받아올 때까지 무한 루프를 돈다.
                 // 현재 버스의 위치를 기반으로 Flag를 업데이트 한다.

    if((Flag[(nowStop == 4) ? 0 : nowStop]) > 0) // 0 1 2 일 경우 앞 정류장에 사용자가 내리려고 한다면, 알람을 울리는 신호를 보내고 벨을 울린다.
    {
      ringAlarm(); // 폰으로 한 정류장 전에 알람을 전송
      ringBell(); // 벨 울림
      Flag[nowStop + 1] = 0; // 다음 정류장의 플래그를 초기화
    }*/
    //updateFlag();
     success = nfc.inListPassiveTarget();
    if(success)updateFlag();
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

int updateFlag(void){
  //Serial.println("Found!");
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, responseLength);
    
    if(success) {
      //Serial.println("Success");
      buzzerNFC();//성공시 부저
      
      if(reading())
        Serial.println("Overflow");//최대치 넘어서면 Overflow
    }
    else {
      //Serial.println("Failed sending SELECT AID");
      buzzerFail();
      Serial.println("Fail");
    }

  switch(desti) {
    case '1': Flag[0]++;
    case '2': Flag[1]++;
    case '3': Flag[2]++;
    case '4': Flag[3]++;
  }
  if(nowStop + 1 == Desti) Flag[nowStop - '1']++;
  flag = 1;
  Serial.println("updateFlag");
}

void reading(void){
    //if(count > MAX) return 1;//최대치를 넘어섰을 때 조건문에 걸림
    
    //Serial.println("desti");
    
    if(!count) {
      user[count].desti = response[2];
      user[count].id[0] = (char)response[3];
      user[count].id[1] = (char)response[4];
      
      Serial.println("count = 0");
    }
    user[count].desti = response[2];
    for(int i = 0;i < 16;i++) {
      user[count].id += (char)response[i + 3];
      Serial.println(" ");
    } //String형 user ID
    
    Serial.println("id");
    
    Serial.print("Destination : ");
    Serial.println(user[count].desti);
    Serial.print("ID : ");
    Serial.println(user[count].id); //디버깅을 위한 출력문
    
    count++; //다음 사용자 인덱스 - 미사용
    return 0;
}

void checkStop(void) {
  breakpoint = false;
  
  while(breakpoint != true) {
    nowStop = Serial.read() - '0'; /* Serial.read 함수는 버퍼가 비었을때 음수를 리턴하므로 switch문으로 음수를 걸러낸다. 
                                      준영이 단에서 코드를 전송했을때는 breakpoint로 함수를 빠져나간다. 그렇지 않을 경우 무한루프를 돈다. */   
    switch(nowStop)
    {
      case 1: case 2: case 3: case 4:
      breakpoint = true;

      default: continue;
    }
  }
}
