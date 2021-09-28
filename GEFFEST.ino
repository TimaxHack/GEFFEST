#include <Servo.h>
#include <SoftwareSerial.h>

//SoftwareSerial btSerial(2, 3);
#define btSerial Serial1

Servo servo1_1;
Servo servo1_2;
Servo servo1_3;
Servo servo2_1;
Servo servo2_2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;

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





void setup(){
   btSerial.begin(38400);
  Serial.begin(38400);

  Serial.println("begin servo");

  targetAngle[0] = 90;
  targetAngle[1] = 90;
  targetAngle[2] = 90;
  targetAngle[3] = 90;
  targetAngle[4] = 90;
  targetAngle[5] = 90;

  servo1_1.attach(2);
  servo1_2.attach(3);
  servo1_3.attach(4);
  servo2_1.attach(5);
  servo2_2.attach(6);
  servo3.attach(7);
  servo4.attach(8);
  servo5.attach(9);
  servo6.attach(10);

  Serial.println("servo started");}

  kilibrocka();

void loop(){}





void kilibrocka() //калибровка mpu
{}
void data_from_serial() //собераем данные с бодов
{}
void parse_data() // парсинг данных(фильтрация), кванторнионы
{}
void set_target_position() // выстовление позиций для серво
{}
void update_geffa() // сглаживание 
{}
