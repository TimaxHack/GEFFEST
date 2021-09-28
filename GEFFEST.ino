
#include "ServoSmooth.h"
//#include <Servo.h>
#include <SoftwareSerial.h>
#define AMOUNT 3 
#define BMOUNT 2
#define CMOUNT 1
#define DMOUNT 1
#define VMOUNT 1
#define NMOUNT 1
#include "ServoSmooth.h"
ServoSmooth servo1[AMOUNT];
ServoSmooth servo2[BMOUNT];
ServoSmooth servo3[CMOUNT];
ServoSmooth servo4[DMOUNT];
ServoSmooth servo5[VMOUNT];
ServoSmooth servo6[NMOUNT];




const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];   // temporary array for use when parsing

// variables to hold the parsed data
int xRecieve1 = 90;
int xRecieve2 = 90;
int yRecieve1 = 90;
int yRecieve2 = 90;
int flexRecieve1 = 90;
int flexRecieve2 = 90;

boolean newData = false;

float currentAngle[6] = { 90,90,90,90,90,90 };
float targetAngle[6] = { 90,90,90,90,90,90 };
float moveSteps = 50;

//============


void setup() {
//   btSerial.begin(38400);
  Serial.begin(38400);

  Serial.println("begin servo");

  targetAngle[0] = 90;
  targetAngle[1] = 90;
  targetAngle[2] = 90;
  targetAngle[3] = 90;
  targetAngle[4] = 90;
  targetAngle[5] = 90;

  servo1[1].attach(2);
  servo1[2].attach(3);
  servo1[3].attach(4);
  servo2[4].attach(5);
  servo2[5].attach(6);
  servo3[6].attach(7);
  servo4[7].attach(8);
  servo5[8].attach(9);
  servo6[9].attach(10);

  Serial.println("servo started");


  servo1[1].setAccel(0.1);
  servo1[2].setAccel(0.1);
  servo1[3].setAccel(0.1);
  servo2[4].setAccel(0.1);
  servo2[5].setAccel(0.1);
  servo3[6].setAccel(0.1);
  servo4[7].setAccel(0.1);
  servo5[8].setAccel(0.1);
  servo6[9].setAccel(0.1);

}

void loop() {


  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    delay(10);
    rc = Serial.read();
    //Serial.print(rc);
   
    delay(10);

    if (recvInProgress == true) {
      if (rc != endMarker) {
        if (rc == startMarker) continue;
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }


  
 // get_data_fromserial(); // проверили, есть ли данные на серийном порту и приняли их с маркерами
  if (newData == true)
  {

    strcpy(tempChars, receivedChars);

    newData = false;
    parseData(); // спарсили в переменные
    //Serial.println("parse data");
    /*
    Serial.print(xRecieve1);
    Serial.print("\t");
    Serial.print(xRecieve2);
    Serial.print("\t");
    Serial.print(yRecieve1);
    Serial.print("\t");
    Serial.print(yRecieve2);
    Serial.print("\t");
    Serial.print(flexRecieve1);
    Serial.print("\t");
    Serial.println(flexRecieve2);
    */

    
    xRecieve1 = constrain(xRecieve1,  -5, 110);
    xRecieve1 = map(xRecieve1,  -5, 110, 0, 180);
    targetAngle[3] = xRecieve1;

    
    xRecieve2 = constrain(xRecieve2,  -180, 180);
    xRecieve2 = map(xRecieve2,  -180, 180, 0, 180);
    targetAngle[5] = xRecieve2;
    //targetAngle[4] = 90;

    yRecieve2 = constrain(yRecieve2,  -90, 90);
    yRecieve2 = map(yRecieve2,  -90, 90, 0, 180);

    yRecieve1 = constrain(yRecieve1,  -90, 40);
    yRecieve1 = map(yRecieve1,  -90, 40, 0, 180);
    targetAngle[1] = yRecieve1;
    
    targetAngle[0] = yRecieve2;
    
    flexRecieve1 = constrain(flexRecieve1,  0, 180);
    targetAngle[5] = flexRecieve1;

    flexRecieve2 = constrain(flexRecieve2,  0, 180);
    targetAngle[2] = flexRecieve2;

    


    int currentAngle1 = 180 - currentAngle[1];
    int currentAngle2 = 180 - currentAngle[2];
       // желаемая позиция задаётся методом setTarget (импульс) или setTargetDeg (угол), далее
  // при вызове tick() производится автоматическое движение сервы
  // с заданным ускорением и ограничением скорости
    uint32_t servoTimer;
    uint32_t turnTimer;
  if (millis() - servoTimer  >= 40) {
    servoTimer = millis();
    //int newPos = map(analogRead(A2), 0, 1023, 500, 2400); // берём с потенцометра значение 500-2400 (импульс)
    servo1[1].setTarget(currentAngle[0]);               // и отправляем на серво
    servo1[2].setTarget(currentAngle[0]);
    servo1[3].setTarget(currentAngle[0]);
    servo2[4].setTarget(currentAngle1);
    servo2[5].setTarget(currentAngle[1]);
    servo3[6].setTarget(currentAngle2);
    servo4[7].setTarget(currentAngle[3]);
    servo5[8].setTarget(currentAngle[4]);
    servo6[9].setTarget(currentAngle[5]);
    //Serial.println(String(newPos) + " " + String(servo.getCurrent())/* + " " + String(state)*/);
  // state показывает сотояние сервы (0 - движется, 1 - приехали и отключились)
  }



    

    
  

}

/*void get_data_fromserial(){
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while ( .available() > 0 && newData == false)
  {
    delay(10);
    rc = Serial.read();
    //Serial.print(rc);
    
    delay(10);

    if (recvInProgress == true) {
      if (rc != endMarker) {
        if (rc == startMarker) continue;
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
  }

  */
  }
void parseData(){
    char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  xRecieve1 = atoi(strtokIndx); // convert this part to a int, atoi =  перевод в int

  strtokIndx = strtok(NULL, ",");
  xRecieve2 = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  yRecieve1 = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  yRecieve2 = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  flexRecieve1 = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  flexRecieve2 = atoi(strtokIndx);
}

long mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}
/*void ServoMove(){  for (int i = 0; i < 6; i++)
  {
    float difference = targetAngle[i] - currentAngle[i];
    currentAngle[i] += difference / moveSteps;
    Serial.print(currentAngle[i]);
    Serial.print("  ");
  }





  








/*
  servo[1].write(currentAngle[0]);
  servo[1].write(currentAngle[0]);
  servo[1].write(currentAngle[0]);

  servo[1].write(180 - currentAngle[1]);
  servo[1].write(currentAngle[1]);

  servo[1].write(180-currentAngle[2]);

  servo[1].write(currentAngle[3]);

  servo[1].write(currentAngle[4]);

  servo[1].write(currentAngle[5]);}*/
  
