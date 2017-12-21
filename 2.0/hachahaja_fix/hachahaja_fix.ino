char desti; // 목적지 코드
int flag[NUMBUS] = {0};
char now; // 현재 버스 위치 코드를 받기 위한 변수
bool breakpoint = false; // breakpoint (checkStop());


void loop() {
  for(int i = 0; i < NUMBUS; i++){
    breakpoint = false;
    now = DirectSerial.read();
    
    while(!breakpoint) {
      
      switch(now){
      case '1': case '2': case '3': case '4':
      breakpoint = true;
      
      default: continue;
      }
    }
    if(flag[i+1] > 0)
      BTSerial.write(ALARM_SIGNAL);
    if(flag[i] > 0){
      buzzer();
      flag[i] = 0;
    }
  }
}
