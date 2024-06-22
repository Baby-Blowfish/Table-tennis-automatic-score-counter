#include <TM1637Display.h>
#include <IRremote.h>
#include "LedControl.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "Arduino.h"
//#define SUPPRESS_ERROR_MESSAGE_FOR_BEGIN
#define FND_DIO 9
#define FND_CLK 8
#define FND_CS 7
#define RED_UP_switch 58
#define RED_DOWN_switch 59
#define timer_switch 56
#define set_reset_switch 57
#define BLUE_UP_switch 54
#define BLUE_DOWN_switch 55
#define IR_RECEIVE_PIN 5
#define timer_CLK 4
#define timer_DIO 3

int RED_score =0;
int RED_set =0;
int BLUE_score =0;
int BLUE_set =0;

//IRrecv irrecv(IR_RECEIVE_PIN);             //적외선 수신용
//decode_results results;

LedControl lc = LedControl(FND_DIO, FND_CLK, FND_CS, 1);    //MOSI, CLK, CS   선언

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

TM1637Display display(timer_CLK, timer_DIO);

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
long BLUE_UP_DebounceTime=0;
long BLUE_DOWN_DebounceTime=0;
long RED_UP_DebounceTime=0;
long RED_DOWN_DebounceTime=0;
long set_reset_switch_DebounceTime=0;
long debounceDelay=20;
int state[5]={0,};
int laststate[5]={0,};
int Button[5]={0,};



void timer(int readTime){           //타이머 출력
  sec = readTime%60;
  minu = (readTime/60)%60;
  hour = (readTime/(60*60))%24;

   if (sec == 5)                    //시간 출력부분인듯 하니, 변수를 minu나 hour로 바꾸면 분,시간마다 출력이 가능할듯함.
  {
    myDFPlayer.play(1);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

  else if (sec == 10)
  {
    myDFPlayer.play(2);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

  else if (sec == 15)
  {
    myDFPlayer.play(3);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

  else if (sec == 20)
  {
    myDFPlayer.play(4);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

  else if (sec == 25)
  {
    myDFPlayer.play(5);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }

  else if (sec == 30)
  {
    myDFPlayer.play(6);
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());
    }
  }
  //display.showNumverDecEx(x,0,ture,2,2) x부분이 실제 출력 부분! 
  //x자리에 초를 표시하고 싶으면 sec대입, 분을 대입하고싶으면 minu대입, 시간을 대입하고싶으면 hour대입!
  display.showNumberDecEx(minu, 0, true,2,2) ;                 
  display.showNumberDecEx(hour, (0x80 >> 1), false, 2, 0) ;  // turn on colon    원본은  0x80 >> 3 인데 콜론이 on 되지 않아 0x80 >> 1 로 변경
}                              //숫자와 숫자사이 :표시 >> (0x80>>1)

void FND_display(){         //점수, 세트 스코어 출력
  if(BLUE_score<0) BLUE_score=0;
  if(BLUE_set<0) BLUE_set=0;
  if(RED_score<0) RED_score=0;
  if(RED_set<0) RED_set=0;  

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

void SW_Score(int sw_name, int num, long DebounceTime, int *score, int i) // i=1이면 업카운트 아니면 다운 카운트
{
  state[num]=digitalRead(sw_name);
  //점수 
  if(state[num]!=laststate[num]){
    DebounceTime=millis();
  }
  if((millis()-DebounceTime)>debounceDelay)
  {
    if(state[num]!=Button[num])
    {
      Button[num]=state[num];
      
      if(Button[num]==HIGH)
      {
        if(i>=1)
        {
          ++*score;
          FND_display();
        }
        else
        {
          --*score;
          FND_display();
        }
       
      }
    }
  }
  laststate[num]=state[num];
}

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  display.setBrightness(7);
  display.showNumberDecEx(0, 0x40, true);    //64
  IrReceiver.enableIRIn();//적외선 수신on
  lc.shutdown(0, false);  // 도트매트릭스의 절전모드를 꺼줍니다.
  lc.setIntensity(0, 15);  // 도트매트릭스의 LED 밝기를 8로 지정해 줍니다.(0~15)
  lc.clearDisplay(0);     // 도트매트릭스의 LED를 초기화 해줍니다.

  pinMode(timer_switch,INPUT_PULLUP);
  pinMode(set_reset_switch,INPUT_PULLUP);
  pinMode(RED_UP_switch,INPUT_PULLUP);
  pinMode(RED_DOWN_switch,INPUT_PULLUP);
  pinMode(BLUE_UP_switch,INPUT_PULLUP);
  pinMode(BLUE_DOWN_switch,INPUT_PULLUP);
  FND_display();

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
}

void loop() 
{
  // score count
  SW_Score(BLUE_UP_switch,0,BLUE_UP_DebounceTime,&BLUE_score,1); 
  SW_Score(BLUE_DOWN_switch,1,BLUE_DOWN_DebounceTime,&BLUE_score,0);
  SW_Score(RED_UP_switch,2,RED_UP_DebounceTime,&RED_score,1);
  SW_Score(RED_DOWN_switch,3,RED_DOWN_DebounceTime,&RED_score,0);
  
  // IR score count
  if(IrReceiver.decode()){ // 적외선 수신 받으면
   Serial.println(IrReceiver.decodedIRData.decodedRawData,HEX);//시리얼로 적외선 신호값 출력
  
  switch(IrReceiver.decodedIRData.decodedRawData)
    {
      case 0xD02F8778: ++BLUE_score;break;    //청팀UP버튼 받았을때. 
      case 0xDF028778: --BLUE_score;break;     //청팀DOWN버튼 받았을때.
      case 0xD02F47F8: ++RED_score;break;    //홍팀UP버튼 받았을때.
      case 0xDF0247B8: --RED_score;break;     //홍팀DOWN버튼 받았을때.
      case 0x1EE150FB: Serial.println("net");break;
      default: break;
    }
  IrReceiver.resume();
  FND_display();  
  }

  // score set count
  if(RED_score>10||BLUE_score>10){ 
    if((RED_score-BLUE_score)>=2){
      ++RED_set;
      RED_score=0;      
      BLUE_score=0;
      FND_display();   
      //스피커출력
    }
    else if((BLUE_score-RED_score)>=2){
      ++BLUE_set;
      RED_score=0;      
      BLUE_score=0;
      FND_display();      
      //스피커출력
    }    
  }

  // score reset
  if(digitalRead(set_reset_switch)==LOW)
  {
    RED_score =0;
    RED_set =0;
    BLUE_score =0;
    BLUE_set =0;
    FND_display();
  }


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
    case 0:
      break;
    case 1:       //시작, 일시정지
      if(Flag==0)timer(cnt);
      else if(Flag==1){
        Serial.println(millis() - timeVal);
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


void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}