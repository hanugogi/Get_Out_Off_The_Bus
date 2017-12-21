 #include <SoftwareSerial.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532Interface.h>
#include <PN532.h>

#define NUM_BUS_STOP 5
#define ALARM_SIGNAL ('5')
#define BTRX 2
#define BTTX 3
#define DTRX 4
#define DTTX 5
#define BUZZER 6
#define MAX 10  //총 User 수 - 미사용

SoftwareSerial BTSerial(BTRX, BTTX);
SoftwareSerial DirectSerial(DTRX, DTTX);
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

bool success;
int count = 0;
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

void setup()
{   
    Serial.begin(115200);
    Serial.println("-------Peer to Peer HCE--------");
    nfc.begin();
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata){
      Serial.println("Can't find PN532");
      while (1);
    }
    Serial.println("Found chip PN532");
    nfc.setPassiveActivationRetries(0x8F);
    nfc.SAMConfig();
    
    BTSerial.begin(9600);
    DirectSerial.begin(9600);
    pinMode(BUZZER, OUTPUT);
    tone(BUZZER, 1000);
    noTone(BUZZER);
    User users[MAX];
    user = &users[0];
}

void loop()
{
  success = nfc.inListPassiveTarget();//데이터를 읽어올 타겟이 있는지 확인
  if(success)
    checking();//데이터 checking
}

void buzzerNFC(void) {
  tone(BUZZER, 4500);
  delay(200);
  tone(BUZZER, 5500);
  delay(150);
  noTone(BUZZER);
}

void buzzerFail(void){
  tone(BUZZER, 2500);
  delay(80);
  noTone(BUZZER);
}

void checking(void){
  //Serial.println("Found!");
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, responseLength);
    if(success) {
      //Serial.println("Seccess");
      buzzerNFC();//성공시 부저
      if(reading())
        Serial.println("Overflow");//최대치 넘어서면 Overflow
    }
    else{
      //Serial.println("Failed sending SELECT AID");
      buzzerFail();
    }
}

boolean reading(void){
    if(count==MAX)
      return 1;//최대치를 넘어섰을 때 조건문에 걸림
    user[count].desti = response[1]-48;//int형 destination
    //Serial.println("desti");
    for(int i=0;i<16;i++){
      user[count].id += (char)response[i+3];
      Serial.println(" ");
    }//String형 user ID
    //Serial.println("id");
    if(count==0){
      user[count].desti = response[1]-48;
      user[count].id[0] = (char)response[3];
      user[count].id[1] = (char)response[4];
      Serial.println("count = 0");
    }
  Serial.print("Destination : ");
  Serial.println(user[count].desti);
  Serial.print("ID : ");
  Serial.println(user[count].id);//디버깅을 위한 출력문
  count++;//다음 사용자 인덱스 - 미사용
  return 0;
}
