// BLUE Button
#include <IRremote.h>
#include <util/delay.h>
#define UP 5          //up스위치
#define DOWN 4        //down스위치
#define DELAY 20

int Send_pin = 3;
int Knock = 2;        //진동센서
void setup()
{
  Serial.begin(9600);
  Serial.println("Start");
  IrSender.begin(Send_pin, ENABLE_LED_FEEDBACK,USE_DEFAULT_FEEDBACK_LED_PIN); // 송신기 작동 시작
  pinMode(UP,INPUT_PULLUP);
  pinMode(DOWN,INPUT_PULLUP);
  pinMode(Knock,INPUT);
  
}

void loop(){
  if(digitalRead(UP)==LOW)                    //up
  {
    _delay_ms(DELAY);
    while(digitalRead(UP)==LOW);
    _delay_ms(DELAY);
    IrSender.sendNECMSB(0x1EE1F40B,32);       //주소값만 수정해주면 
    Serial.println("Start");
  }
  else if(digitalRead(DOWN)==LOW)             //down
  {
    _delay_ms(DELAY);
    while(digitalRead(DOWN)==LOW);
    _delay_ms(DELAY);
    IrSender.sendNECMSB(0x1EE140FB,32);       //주소값만 수정해주면 끝
    Serial.println("Start");
  }
  else if(digitalRead(Knock)==LOW)                    //진동센서 감지시
  {
    IrSender.sendNECMSB(0x1EE150FB,32);       //주소값만 수정해주면 끝
  }
}
