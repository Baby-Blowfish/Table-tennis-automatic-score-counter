
#include <IRremote.h>
#define UP 5          //up스위치
#define DOWN 6        //down스위치
int Send_pin = 3;
void setup()
{
  Serial.begin(9600);
  Serial.println("Start");
  IrSender.begin(Send_pin, ENABLE_LED_FEEDBACK); // 송신기 작동 시작
  pinMode(UP,INPUT_PULLUP);
  pinMode(DOWN,INPUT_PULLUP);
}

/*void loop() {
  IrSender.sendNECMSB(0x1EE1F40B,32);
  delay(1000);
  IrSender.sendNECMSB(0x1EE140FB,32);
  delay(1000);
}*/
void loop(){
  if(digitalRead(UP)==LOW)                    //up
  {
    while(digitalRead(UP)==LOW);
    IrSender.sendNECMSB(0x1EE1F40B,32);       //주소값만 수정해주면 끝
  }
  else if(digitalRead(DOWN)==LOW)             //down
  {
    while(digitalRead(DOWN)==LOW);
    IrSender.sendNECMSB(0x1EE140FB,32);       //주소값만 수정해주면 끝
  }
}
