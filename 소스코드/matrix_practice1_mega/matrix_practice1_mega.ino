
#include "LedControl.h"

#define FND_DIO 9
#define FND_CLK 8
#define FND_CS 7
#define RED_UP_switch 54
#define RED_DOWN_switch 55
#define timer_switch 56
#define set_reset_switch 57
#define BLUE_UP_switch 58
#define BLUE_DOWN_switch 59

 
int RED_score =0;
int RED_set =0;
int BLUE_score =0;
int BLUE_set =0;

LedControl lc = LedControl(FND_DIO, FND_CLK, FND_CS, 1);    //MOSI, CLK, CS   선언

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

void setup()
{
  // 도트매트릭스의 절전모드를 꺼줍니다.
  lc.shutdown(0, false);
 
  // 도트매트릭스의 LED 밝기를 8로 지정해 줍니다.(0~15)
  lc.setIntensity(0, 8);
  
  // 도트매트릭스의 LED를 초기화 해줍니다.
  lc.clearDisplay(0);

  
  pinMode(timer_switch,INPUT_PULLUP);
  pinMode(set_reset_switch,INPUT_PULLUP);
  pinMode(RED_UP_switch,INPUT_PULLUP);
  pinMode(RED_DOWN_switch,INPUT_PULLUP);
  pinMode(BLUE_UP_switch,INPUT_PULLUP);
  pinMode(BLUE_DOWN_switch,INPUT_PULLUP);
}
 
void loop()
{
  FND_display(); //FND 출력
  delay(10);
  //점수
  if(digitalRead(BLUE_UP_switch)==LOW){
    while(digitalRead(BLUE_UP_switch)==LOW);
    ++BLUE_score;      
  }
  else if(digitalRead(BLUE_DOWN_switch)==LOW){
    while(digitalRead(BLUE_DOWN_switch)==LOW);
    --BLUE_score;      
  }
  else if(digitalRead(RED_UP_switch)==LOW){
    while(digitalRead(RED_UP_switch)==LOW);
    ++RED_score;      
  }
  else if(digitalRead(RED_DOWN_switch)==LOW){
    while(digitalRead(RED_DOWN_switch)==LOW);
    --RED_score;      
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

  /*
  lc.clearDisplay(0); // To Clear the display register 
  lc.setDigit(0,7,7, false); // To display a 1 at the first column on the left (column 7) 
  lc.setDigit(0,6,6, false); // To display a 2 column 6 and decimal point
  lc.setDigit(0,5,5, false); // display a "4" in column 4
  lc.setDigit(0,4,4, false); // display a "4" in column 4
  lc.setDigit(0,3,3, false); // display a "2" in column 3
  lc.setDigit(0,2,2, false); // To display a 3 column 1
  lc.setDigit(0,1,1, false); // 6 show a first hand column, column 0
  lc.setDigit(0,0,0, false); // 6 show a first hand column, column 0
  */
}