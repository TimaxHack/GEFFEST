//#include <Adafruit_PWMServoDriver.h>
#include <ServoDriverSmooth.h>
#include <ServoSmooth.h>
#include <smoothUtil.h>
//#include <FABRIK2D.h>
#include <Multiservo.h>
#include "EEPROM.h"
#include <Wire.h>
#include <Kinematics.h>

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

#define FOR_i(from, to) for(int i = (from); i < (to); i++)
///////////////////////////////////   CONFIGURATION   /////////////////////////////
// ПИНЫ ДЛЯ ДАТЧИКОВ
#define TUM_PIN 15

// ПИНЫ ДЛЯ СЕРВ
#define S_Y   0
#define S_P1  1
#define S_P2  2
#define S_P3  3
#define S_P4  4
#define E_1   5
#define E_2   6
#define W_P   7
#define W_R   8
#define F     9

#define JOINT1_LENGTH 200
#define JOINT2_LENGTH 200

// ПРЕДЕЛЬНЫЕ УГЛЫ ДЛЯ ОСЕЙ
#define SY_MIN  0 
#define SY_MAX    180
#define SP_MIN  0 
#define SP_MAX    180
#define E_MIN   0 
#define E_MAX     180
#define WP_MIN  0 
#define WP_MAX    180
#define WR_MIN  0 
#define WR_MAX    180
#define F_MIN   0 
#define F_MAX     30

// КОЛ-ВО СЕРВ

#define MULTI_SERVO_COUNT 10

// КОЛ-ВО СЕРВ
#define SMOOTH_SERVO_SPEED 90
#define SMOOTH_SERVO_ACCEL 0.5

enum Mode {START, WORK, DEMO, DEBUG, ENDING} mode;


char divider = ' ';
char ending = ';';

const char *headers[]  = {
  "set_sy",   // 0
  "set_sp",   // 1
  "set_e",    // 2
  "set_wp",   // 3
  "set_wr",   // 4
  "set_f",    // 5
  "set_x",    // 6
  "set_y",    // 7
  "set_z",    // 8
};
enum names {
  AXIS_SH_YAW,     // 0
  AXIS_SH_PITCH,   // 1
  AXIS_ELBOW,      // 2
  AXIS_WR_PITCH,   // 3
  AXIS_WR_ROLL,    // 4
  AXIS_FIST,       // 5
  POS_X,           // 6
  POS_Y,           // 7
  POS_Z,           // 8
};

names command;
byte headers_am = sizeof(headers) / 2;
uint32_t prsTimer;
String prsValue = "";
String prsHeader = "";
enum stages {WAIT, HEADER, GOT_HEADER, VALUE, SUCCESS};
stages parseStage = WAIT;
boolean recievedFlag;

Kinematics k(JOINT1_LENGTH, JOINT2_LENGTH);
Position p;
Angle a;
  
void setup() {
  Serial1.begin(115200);
  Serial.begin(9600);  
  
  p = k.getPositions();
  a = k.getAngles();

  
  // Перебираем значения моторов
  FOR_i(0, MULTI_SERVO_COUNT){
    // Подключаем сервомотор
    multiservo[i].attach(i);
    servosmooth[i].attach(i);
    servosmooth[i].setSpeed(SMOOTH_SERVO_SPEED);
    servosmooth[i].setAccel(SMOOTH_SERVO_ACCEL);
  }

  //setMode(START);

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
   
  p = k.getPositions();
  a = k.getAngles();

  
  // Перебираем значения моторов
  FOR_i(0, MULTI_SERVO_COUNT){
    // Подключаем сервомотор
    multiservo[i].attach(i);
    servosmooth[i].attach(i);
    servosmooth[i].setSpeed(SMOOTH_SERVO_SPEED);
    servosmooth[i].setAccel(SMOOTH_SERVO_ACCEL);
  }

  //setMode(START);
      
}
bool flag;

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
  servosmooth[5].setTargetDeg(pos[3]);
  servosmooth[6].setTargetDeg(pos[4]);
  servosmooth[7].setTargetDeg(pos[5]);
}

void getData(){
     if (Serial1.available() > 0){
      char state;
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

void writeAngle(int axis, int value){
  constrainAngles(axis, value);
  switch(axis){
    case 0: 
      multiservo[S_Y].    write(value);         servosmooth[S_Y].   setCurrentDeg(value);
    break;
    case 1:
      multiservo[S_P1].   write(value);         servosmooth[S_P1].  setCurrentDeg(value);
      multiservo[S_P2].   write(180-value);     servosmooth[S_P2].  setCurrentDeg(180-value);
      multiservo[S_P3].   write(value);         servosmooth[S_P3].  setCurrentDeg(value);
      multiservo[S_P4].   write(180-value);     servosmooth[S_P4].  setCurrentDeg(180-value);
    break;
    case 2:
      multiservo[E_1].    write(value);         servosmooth[E_1].   setCurrentDeg(value);
      multiservo[E_2].    write(180-value);     servosmooth[E_2].   setCurrentDeg(180-value);
    break;
    case 3:
      multiservo[W_P].    write(value);         servosmooth[W_P].   setCurrentDeg(value);
    break;
    case 4:
      multiservo[W_R].    write(value);         servosmooth[W_R].   setCurrentDeg(value);
    break;
    case 5:
      multiservo[F].      write(value);         servosmooth[F].     setCurrentDeg(value);
    break;
  }
}

void setTargetAngle (int sy, int sp, int e, int wp, int wr, int f){
    
    
  
}




int constrainAngles(int axis, int value){
  int min = 0, max = 0;
  switch(axis){
    case 0: min = SY_MIN; max = SY_MAX;
    break;
    case 1: min = SP_MIN;  max = SP_MAX;
    break;
    case 2: min = E_MIN;   max = E_MAX;
    break;
    case 3: min = WP_MIN;  max = WP_MAX;
    break;
    case 4: min = WR_MIN;  max = WR_MAX;
    break;
    case 5: min = F_MIN;   max = F_MAX;
    break;
  }
  return constrain(value,min,max);
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
