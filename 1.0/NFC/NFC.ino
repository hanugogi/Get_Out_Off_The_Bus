//#define SUM(A, B) (A ## B)

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <SoftwareSerial.h>

PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

SoftwareSerial BTSerial(2, 3);

typedef struct User{
  int destination;
  String ID;
};

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");
  BTSerial.begin(9600);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find NFC module");
    while (1); // halt
  }
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  Serial.println("Connected with the NFC module\n");
}

void loop(void) {
  readNFC();
}

void readNFC(){
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  int count = 0;
  //User *users[7];
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value(HEX): ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" ");Serial.print(uid[i], HEX);Serial.print(" ");
    }Serial.println(" ");

    Serial.print("UID Value(DEC): ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" ");Serial.print(uid[i], DEC);
    }Serial.println(" ");
    
    //users[count++] = makeID(uid, uidLength);
    delay(1000);
  }
  else{
    Serial.println("Waiting for a card");
    delay(100);
  }
}
/*User* makeID(uint8_t *uid, uint8_t uidLength){
  int destination;
  char ID[16];
  
  //내용 분석 후 각 변수에 저장
  
  User *temp;
  temp->ID = ID;
  temp->destination = destination;

  return temp;
}*/
