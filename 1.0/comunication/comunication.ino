#include <SoftwareSerial.h>

#define NUMBUS 5
#define ALARMSIGNAL ('5')
#define BELLSIGNAL ('6')
#define BTRX 2
#define BTTX 3
#define DTRX 4
#define DTTX 5

SoftwareSerial BTSerial(BTRX, BTTX);
SoftwareSerial DirectSerial(DTRX, DTTX);

char Desti; // 목적지 코드
int Flag[NUMBUS] = {0}; // Flag 첫 정류장은 
int i, j; //반복문을 위한 변수
char nowStop; // 현재 버스 위치 코드를 받기 위한 변수
bool breakpoint = false; // breakpoint (checkStop());

void ringAlarm(void) { BTSerial.write(ALARMSIGNAL); } // 알람을 보낸다
void ringBell(void) { DirectSerial.write(BELLSIGNAL); } // 벨을 키라는 신호를 보낸다. To 준영
void updateFlag(void) { if(nowStop + 1 == Desti) Flag[nowStop - '1']++; } //현재 Desti를 체크하고 Flag를 업데이트 한다
void checkStop(void); // 현재 버스 위치를 받아온다. From 준영


void setup() {
  BTSerial.begin(9600);
  DirectSerial.begin(9600);
}

void loop() {
  for(i = 0; i < NUMBUS; i++) // 정류장마다 한번씩 검사해준다. 마지막 정류장은 도달하지만 알람을 미리 보낼 정류장이 없으므로 제외한다. 0 = 두번째 정류장 1 = 세번째 정류장 2 = 네번째 정류장
  {
    checkStop(); //준영이 라인에서 현재 버스의 위치를 받아온다. 받아오는데 실패할 경우 받아올 때까지 무한 루프를 돈다.
    updateFlag(); // 현재 버스의 위치를 기반으로 Flag를 업데이트 한다.
    
    if(Flag[i] > 0) // 0 1 2 일 경우 앞 정류장에 사용자가 내리려고 한다면, 알람을 울리는 신호를 보내고 벨을 울린다.
    {
      ringAlarm(); // 폰으로 한 정류장 전에 알람을 전송
      ringBell(); //준영이 라인에서 벨 처리 신호 전송
      Flag[i] = 0; // 다음 정류장의 플래그를 초기화
    }
  }
}

void checkStop(void) {
  breakpoint = false;
  
  while(breakpoint != true) {
    nowStop = DirectSerial.read(); /* Serial.read 함수는 버퍼가 비었을때 음수를 리턴하므로 switch문으로 음수를 걸러낸다. 
                                      준영이 단에서 코드를 전송했을때는 breakpoint로 함수를 빠져나간다. 그렇지 않을 경우 무한루프를 돈다. */   
    switch(nowStop)
    {
      case '1': case '2': case '3': case '4':
      breakpoint = true;

      default: continue;
    }
  }
}
