#include <SoftwareSerial.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532Interface.h>
#include <PN532.h>

#define NUMBUS 5
#define ALARMSIGNAL ('5')
#define BELLSIGNAL ('6')
#define BTRX 2
#define BTTX 3
#define DTRX 4
#define DTTX 5
#define BUZZER 6

SoftwareSerial BTSerial(BTRX, BTTX);
SoftwareSerial DirectSerial(DTRX, DTTX);
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);

void setup()
{   
    Serial.begin(115200);
    Serial.println("-------Peer to Peer HCE--------");

    BTSerial.begin(9600);
    nfc.begin();
    pinMode(BUZZER, OUTPUT);

    BTSerial.write(ALARMSIGNAL);
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.println("Didn't find PN53x board");
      while (1); // halt
    }
    
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0x8F);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
}

void loop()
{
  uint8_t response[64];
  uint8_t responseLength = 64;
  uint8_t selectApdu[] = { 0x00, /* CLA */
                           0xA4, /* INS */
                           0x04, /* P1  */
                           0x00, /* P2  */
                           0x07, /* Length of AID  */
                           (byte)0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                           0x00  /* Le  */ };
  
  Serial.println("Waiting for an ISO14443A card");
  bool success = nfc.inListPassiveTarget();

  if(success) {
    Serial.println("Found something!");
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
    
    if(success) {
      Serial.print("responseLength: "); Serial.println(responseLength);
      nfc.PrintHexChar(response, responseLength);
      
      tone(BUZZER, 7000);
      delay(100);
      tone(BUZZER, 6000);
      delay(100);
      noTone(BUZZER);
      //함수화시키기
      do {
          success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
          
          if(success) 
            nfc.PrintHexChar(response, responseLength);
          else 
            Serial.println("Broken connection?"); 
      }
      while(success);
    }
    else
      Serial.println("Failed sending SELECT AID");
  }
  else
    Serial.println("Didn't find anything!");
  delay(1000);
}





/*
void printResponse(uint8_t *response, uint8_t responseLength) {
  
   String respBuffer;

    for (int i = 0; i < responseLength; i++) {
      
      if (response[i] < 0x10) 
        respBuffer = respBuffer + "0"; //Adds leading zeros if hex value is smaller than 0x10
      
      respBuffer = respBuffer + String(response[i], HEX) + " ";                        
    }

    Serial.print("response: "); Serial.println(respBuffer);
}

void setupNFC() {
 
  nfc.begin();
    
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig(); 
}
*/
