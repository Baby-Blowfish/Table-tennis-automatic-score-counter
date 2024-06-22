#include <TM1637Display.h>
#include <IRremote.h>
#define CLK 8
#define DIO 9
#define SW 4

TM1637Display display(CLK, DIO);
IRrecv irrecv(7);             //적외선 수신용

extern volatile unsigned long timer0_millis;  //millis 함수 초기화변수
unsigned long timeLimit = 3600000;// 1000*60*30 = 60분
int hour, minu, sec;
unsigned long readTime; //FND출력용 시간
unsigned long timeVal=0;//이전시간
int cnt = 0;
int Flag =0;
int counts =0;
unsigned long cnts = 0; //버튼눌리는 시간의 거리
int states = 0;
char turnon='0';
char check='0';
void fnd(int readTime)
{
  sec = readTime%60;
  minu = (readTime/60)%60;
  hour = (readTime/(60*60))%24;
  /*Serial.print(hour);
  Serial.print(":");
  Serial.print(minu);
  Serial.print(":");
  Serial.print(sec);
  Serial.println('\n');*/
  /* As per most tasks, this task is implemented in an infinite loop. */
    display.showNumberDecEx(sec, 0, true,2,2) ;
    // Display the minutes in the first two places, with colon
    display.showNumberDecEx(minu, (0x80 >> 1), false, 2, 0) ;  // turn on colon    원본은  0x80 >> 3 인데 콜론이 on 되지 않아 0x80 >> 1 로 변경
    //showNumberDecEx(표시할번호,도트,선행제로,설정할 자릿수,위치 최하위 숫자(0-맨왼쪽 ,3맨오른쪽)); 
    /*도트
    00:00 (0b01000000) 점과 콜론 콜론이있는 디스플레이의 경우 :
   */ 
}
void setup() {
  Serial.begin(9600);
  display.setBrightness(7);
  pinMode(SW,INPUT_PULLUP);
  display.showNumberDecEx(0, 0x40, true);    //64
  irrecv.enableIRIn();//적외선 수신on
}

void loop() {
  /*if(irrecv.decode(&results))//적외선 수신을 받으면
  {
    Serial.println(results.value,HEX);//시리얼로 적외선 신호값 출력
    switch(results.value)
    {
      case 0x1EE1F40B: digitalWrite(6,HIGH);break;    //청팀UP버튼 받았을때.>>처리할 문장 입력하면 됨.  
      case 0x1EE140FB: digitalWrite(6,LOW);break;     //청팀DOWN버튼 받았을때.>>처리할 문장 입력하면 됨.
      case 0x1EE1F40B: digitalWrite(6,HIGH);break;    //홍팀UP버튼 받았을때.>>처리할 문장 입력하면 됨.  
      case 0x1EE140FB: digitalWrite(6,LOW);break;     //홍팀DOWN버튼 받았을때.>>처리할 문장 입력하면 됨.
      default: break;
    }
    irrecv.resume();
  }*/
  if((digitalRead(SW)==LOW)&&counts==0&&cnts==0)//버튼을 눌리면
  {
    while(digitalRead(SW)==LOW);
    cnts = millis();//현재시간
    counts++;       //누른 횟수
    /*Serial.print("states:");
    Serial.print(states);
    Serial.print("counts:");
    Serial.print(counts);
    Serial.print("\n");*/
  }
  else if(millis() >cnts+1000&&cnts>0)//1초안에 버튼을 누르지 않고 1번이상 눌렀을때
  {
    states = counts;  //누른 횟수 저장
    counts = 0;     //카운트 초기화
    cnts = 0;       //초기화 
    Serial.print("states:");
    Serial.print(states);
    Serial.print("counts:");
    Serial.print(counts);
    Serial.print("\n");
    if(Flag==0)Flag=1;
    else if(Flag==1)Flag=0;
  }
  else if((digitalRead(SW)==LOW)&&millis()<cnts+300)//0.5초안에 한번더 눌리면
  {
    while(digitalRead(SW)==LOW);
    cnts=millis();//시간 초기화
    counts++;     //카운트 증가
    if(counts>=2)counts=2; //3번이상이면 카운트 3 
    /*Serial.print("states:");
    Serial.print(states);
    Serial.print("counts:");
    Serial.print(counts);
    Serial.print("\n");*/
  }
  switch(states)
  {
    case 0:
    break;
    case 1:
      if(Flag==0)fnd(cnt);
      else if(Flag==1)
      {
        if(millis()-timeVal>=1000)
        {
          timeVal = millis();
          cnt++;
        }
        fnd(cnt);
      }
    break;
    case 2:
      cnt=0;
      states=1;
      Flag=0;
    break;
  }
}
