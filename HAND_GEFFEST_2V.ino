#include <Adafruit_PWMServoDriver.h>
#include <ServoDriverSmooth.h>
#include <ServoSmooth.h>
#include <smoothUtil.h>
#include <FABRIK2D.h>
#include <Multiservo.h>

#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define MULTI_SERVO_COUNT 8

int sleep[6] = {90, 0, 180, 90, 90, 30};
int point1[6] = {90, 0, 180, 90, 90, 30};
int point2[6] = {90, 0, 180, 90, 90, 30};
int point3[6] = {90, 0, 180, 90, 90, 30};
int pointGuba[6] = {90, 0, 180, 90, 90, 30};
int pointUnGuba[6] = {90, 0, 180, 90, 90, 30};
int pointMarker[6] = {90, 0, 180, 90, 90, 30};
int pointUnMarker[6] = {90, 0, 180, 90, 90, 30};


ServoDriverSmooth servosmooth[MULTI_SERVO_COUNT];
Multiservo multiservo[MULTI_SERVO_COUNT];

float ang = 0;
float radius = 30;
float x_offset = 150;
float y_offset = 200;

void setup() {
  Serial.begin(9600);
  Serial1.begin(38400);

  FOR_i(0, MULTI_SERVO_COUNT-1){
    multiservo[i].attach(i);
    servosmooth[i].attach(i);
    servosmooth[i].setSpeed(50);
    servosmooth[i].setAccel(0.35);
  }
  multiservo[7].attach(7);
    servosmooth[7].attach(7);
    servosmooth[7].setSpeed(100);
    servosmooth[7].setAccel(0.5);

  multiservo[0].write(90);
  multiservo[1].write(90);
  multiservo[2].write(0);
  multiservo[3].write(180);
  multiservo[4].write(180);
  multiservo[5].write(90);
  multiservo[6].write(90);
  multiservo[7].write(30);

  servosmooth[0].setCurrentDeg(90);
  servosmooth[1].setCurrentDeg(90);
  servosmooth[2].setCurrentDeg(0);
  servosmooth[3].setCurrentDeg(180);
  servosmooth[4].setCurrentDeg(180);
  servosmooth[5].setCurrentDeg(90);
  servosmooth[6].setCurrentDeg(90);
  servosmooth[7].setCurrentDeg(30);

  servosmooth[0].setTargetDeg(90);
  servosmooth[1].setTargetDeg(90);
  servosmooth[2].setTargetDeg(0);
  servosmooth[3].setTargetDeg(180);
  servosmooth[4].setTargetDeg(180);
  servosmooth[5].setTargetDeg(90);
  servosmooth[6].setTargetDeg(90);
  servosmooth[7].setTargetDeg(30);
      
}
bool flag;
int state, mode;
void loop() {
  
  if (mode == 1) {
  setAngles(point1);
  while (!axisIsReady()){
     updateServos();
   }
   delay(250);
   setAngles(point2);
  while (!axisIsReady()){
     updateServos();
   }
   delay(250);
   setAngles(point3);
  while (!axisIsReady()){
     updateServos();
   }
   delay(250);
    mode = 7;
  }
  
  if (mode == 2) {

    
      while (!axisIsReady()){
      updateServos();
      }
    mode = 7;
  }
  
  if (mode == 3) {

    
      while (!axisIsReady()){
      updateServos();
      }
    mode = 7;
  }
  
  if (mode == 4) {

    
  while (!axisIsReady()){
      updateServos();
      }
    mode = 7;    
  }
  
  if (mode == 5) {


  while (!axisIsReady()){
      updateServos();
      }
    mode = 7;    
  }
  
  if (mode == 6) {


  while (!axisIsReady()){
      updateServos();
      }
    mode = 7;
  }
  
  if (mode == 7){
    Serial.println("Sleep");
    setAngles(sleep);
    while (!axisIsReady()){
      updateServos();
      }
    }
}
  
void setAngles(int pos[6]){
  servosmooth[0].setTargetDeg(pos[0]);
  servosmooth[1].setTargetDeg(pos[0]);
  servosmooth[2].setTargetDeg(pos[1]);
  servosmooth[3].setTargetDeg(180 - pos[1]);
  servosmooth[4].setTargetDeg(pos[2]);
  servosmooth[5].setTargetDeg(pos[3);
  servosmooth[6].setTargetDeg(pos[4]);
  servosmooth[7].setTargetDeg(pos[5]);
}

void getData(){
     if (Serial1.available() > 0){
   state = Serial1.read(); // Reads the data from the serial port
      Serial1.print(state); // Prints out the value sent

      switch (state) {
        case 'S': 
        break;
        case 's': 
        break;
        case 'E':
        break;
        case 'e':
        break;

        case 'G': flag=true;
        break;
    }
  }
}

void updateServos(){  
 FOR_i(0, MULTI_SERVO_COUNT){
    servosmooth[i].tick();
    multiservo[i].write(servosmooth[i].getCurrentDeg());
  }
}

bool axisIsReady(){
  int counter = 0;
  int temp;
  FOR_i(0, 10){
    temp = servosmooth[i].getCurrentDeg()-servosmooth[i].getTargetDeg();
      if ( abs(temp) < 8) counter ++;
  }
  return counter == 10;
}
