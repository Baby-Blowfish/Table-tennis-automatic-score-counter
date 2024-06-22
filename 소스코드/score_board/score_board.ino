#include <TM1637Display.h>
#include <IRremote.h>
#include "LedControl.h"

#define FND_DIO 9
#define FND_CLK 8
#define FND_CS 7
#define RED_UP_switch 19
#define RED_DOWN_switch 18
#define timer_switch 17
#define set_reset_switch 16
#define BLUE_UP_switch 15
#define BLUE_DOWN_switch 14

#define timer_CLK 8
#define timer_DIO 9

int RED_score =0;
int RED_set =0;
int BLUE_score =0;
int BLUE_set =0;

LedControl lc = LedControl(FND_DIO, FND_CLK, FND_CS, 1);    //MOSI, CLK, CS   선언

TM1637Display display(timer_CLK, timer_DIO);
IRrecv irrecv(7);             //적외선 수신용

extern volatile unsigned long timer0_millis;  //millis 함수 초기화변수
unsigned long timeLimit = 3600000;// 1000*60*30 = 60분
int hour, minu, sec;
unsigned long readTime; //timer출력용 시간
unsigned long timeVal=0;//이전시간
int cnt = 0;
int Flag =0;
int counts =0;
unsigned long cnts = 0; //버튼눌리는 시간의 거리
int states = 0;
char turnon='0';
char check='0';


// 스위치 디바운싱
int lastDebounceTime=0;


void timer(int readTime){           //타이머 출력
  sec = readTime%60;
  minu = (readTime/60)%60;
  hour = (readTime/(60*60))%24;

  display.showNumberDecEx(sec, 0, true,2,2) ;
  display.showNumberDecEx(minu, (0x80 >> 1), false, 2, 0) ;  // turn on colon    원본은  0x80 >> 3 인데 콜론이 on 되지 않아 0x80 >> 1 로 변경
}

void FND_display(){         //점수, 세트 스코어 출력
  if(BLUE_score<0) BLUE_score=0;
  else if(BLUE_set<0) BLUE_set=0;
  else if(RED_score<0) RED_score=0;
  else if(RED_set<0) RED_set=0;  

  lc.clearDisplay(0);
  lc.setDigit(0,7,BLUE_set/10, false); // 
  lc.setDigit(0,6,BLUE_set%10, false); // 
  lc.setDigit(0,5,RED_set/10, false); //
  lc.setDigit(0,4,RED_set%10, false); // 

  lc.setDigit(0,3,BLUE_score/10, false); // 
  lc.setDigit(0,2,BLUE_score%10, false); // 
  lc.setDigit(0,1,RED_score/10, false); // 
  lc.setDigit(0,0,RED_score%10, false); // 

}


void setup() {
  Serial.begin(9600);
  display.setBrightness(7);
  display.showNumberDecEx(0, 0x40, true);    //64
  irrecv.enableIRIn();//적외선 수신on

  lc.shutdown(0, false);  // 도트매트릭스의 절전모드를 꺼줍니다.
  lc.setIntensity(0, 8);  // 도트매트릭스의 LED 밝기를 8로 지정해 줍니다.(0~15)
  lc.clearDisplay(0);     // 도트매트릭스의 LED를 초기화 해줍니다.

  pinMode(timer_switch,INPUT_PULLUP);
  pinMode(set_reset_switch,INPUT_PULLUP);
  pinMode(RED_UP_switch,INPUT_PULLUP);
  pinMode(RED_DOWN_switch,INPUT_PULLUP);
  pinMode(BLUE_UP_switch,INPUT_PULLUP);
  pinMode(BLUE_DOWN_switch,INPUT_PULLUP);
}

void loop() {
  FND_display(); //FND 출력

  
  
  //점수
  if(digitalRead(BLUE_UP_switch)==LOW){
    lastDebounceTime=millis();




  }
  else if(digitalRead(BLUE_DOWN_switch)==LOW){
    --BLUE_score;      
  }
  else if(digitalRead(RED_UP_switch)==LOW){
    ++RED_score;      
  }
  else if(digitalRead(RED_DOWN_switch)==LOW){
    --RED_score;      
  }

  if(millis()-lastDebounceTime>50){
    ++BLUE_score;
    lastDebounceTime=0;
  }


  //세트 
  if(RED_score>10||BLUE_score>10){ 
    if((RED_score-BLUE_score)>=2){
      ++RED_set;
      RED_score=0;      
      BLUE_score=0;   
      //스피커출력
    }
    else if((BLUE_score-RED_score)>=2){
      ++BLUE_set;
      RED_score=0;      
      BLUE_score=0;      
      //스피커출력
    }    
  }


  //적외선 수신
  /*
  if(irrecv.decode(&results)){
    Serial.println(results.value,HEX);//시리얼로 적외선 신호값 출력
    switch(results.value)
    {
      case 0x1EE1F40B: ++BLUE_score;break;    //청팀UP버튼 받았을때. 
      case 0x1EE140FB: --BLUE_score;break;     //청팀DOWN버튼 받았을때.
      case 0x1EE1F40B: ++RED_score;break;    //홍팀UP버튼 받았을때.
      case 0x1EE140FB: --RED_score;break;     //홍팀DOWN버튼 받았을때.
      default: break;
    }
    irrecv.resume();
  }
 */

  //timer 정지, 리셋
  if((digitalRead(timer_switch)==LOW)&&counts==0&&cnts==0){//버튼을 눌리면
    while(digitalRead(timer_switch)==LOW);
    cnts = millis();//현재시간
    counts++;       //누른 횟수
  }
  else if(millis() >cnts+1000&&cnts>0){//1초안에 버튼을 누르지 않고 1번이상 눌렀을때
    states = counts;  //누른 횟수 저장
    counts = 0;     //카운트 초기화
    cnts = 0;       //초기화 

    if(Flag==0)Flag=1;
    else if(Flag==1)Flag=0;
  }
  else if((digitalRead(timer_switch)==LOW)&&millis()<cnts+300){//0.5초안에 한번더 눌리면
    while(digitalRead(timer_switch)==LOW);
    cnts=millis();//시간 초기화
    counts++;     //카운트 증가
    if(counts>=2)counts=2; //3번이상이면 카운트 3 

  }
  switch(states){

    case 1:       //시작, 일시정지
      if(Flag==0)timer(cnt);
      else if(Flag==1){
        if(millis()-timeVal>=1000){
          timeVal = millis();
          cnt++;
        }
        timer(cnt);
      }
    break;
    case 2:        //초기화
      cnt=0;
      states=1;
      Flag=0;
    break;
  }
}
