#define COMPL_K 0.07
#include "Wire.h"
// получаем углы при помощи DMP
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"
#include "Wire.h"

#include <SoftwareSerial.h>
SoftwareSerial btSerial(2, 3);

unsigned long lastSendTime;
int sendDelay = 500;

MPU6050 mpu;

float angleX = 0;
float angleY = 0;
float angleZ = 0;

float angleX1 = 0;
float angleY1 = 0;
float angleZ1 = 0;

float angleX2 = 0;
float angleY2 = 0;
float angleZ2 = 0;
boolean b = false;

int flex1pin = A0;
int flex2pin = A1;

int flex1min, flex1max, flex2min, flex2max;
int flex1value, flex2value;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(flex1pin, INPUT);
  pinMode(flex2pin, INPUT);

  digitalWrite(13, HIGH);
  delay(2000);
  flex1max = analogRead(flex1pin);
  flex2max = analogRead(flex2pin);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(2000);
  flex1min = analogRead(flex1pin);
  flex2min = analogRead(flex2pin);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  
  Wire.begin();
  Wire.setClock(400000ul);
  Serial.begin(38400);
  mpu.initialize();
  initDMP();
  pinMode(12, OUTPUT);
  pinMode(8, OUTPUT);

  btSerial.begin(38400);
}

void loop()
{
  if (b == true)
  {
    // смена адреса модуля
    digitalWrite(12, HIGH);
    digitalWrite(8, LOW);
    
    getAngles();
    Serial.print("MPU pin12: ");
    Serial.print(angleX); Serial.print("\t\t");
    Serial.print(angleY); Serial.print("\t\t");
    Serial.print(angleZ); Serial.print("\t\t");

    // копируем полученные из getAngles() углы в глобальные переменные для последующей отправки
    angleX1 = angleX;
    angleY1 = angleY;
    angleZ1 = angleZ;
    delay(20);
    b = false;
  }
  else
  {
    // смена адреса модуля
    digitalWrite(12, LOW);
    digitalWrite(8, HIGH);
    
    getAngles();
    Serial.print("MPU pin8: ");
    Serial.print(angleX); Serial.print("\t\t");
    Serial.print(angleY); Serial.print("\t\t");
    Serial.print(angleZ); Serial.print("\t\t");

    // копируем полученные из getAngles() углы в глобальные переменные для последующей отправки
    angleX2 = angleX;
    angleY2 = angleY;
    angleZ2 = angleZ;
    delay(20);
    b = true;

    flex1value = analogRead(flex1pin);
    flex1value = map(flex1value, flex1min, flex1max, 0, 180);
    flex2value = analogRead(flex2pin);
    flex2value = map(flex2value, flex2min, flex2max, 0, 180);
    

    Serial.print(flex1value); Serial.print("\t\t");
    Serial.println(flex2value);
  }
  delay(10);

  if (millis() > lastSendTime + sendDelay) // отправка через блютуз идет по собственному "таймеру", каждые sendDelay миллисекунд
  {
    lastSendTime = millis();
    
    // send
    Serial.println("Send");
    btSerial.print("<");
    btSerial.print(int(angleX1));
    btSerial.print(",");
    btSerial.print(int(angleZ1));
    btSerial.print(",");
    btSerial.print(int(angleY2));
    btSerial.print(",");
    btSerial.print(int(angleZ2));
    btSerial.print(",");
    btSerial.print(int(flex1value));
    btSerial.print(",");
    btSerial.print(int(flex2value));
    btSerial.print(">");
  }
}

// НУЖНЫЕ ПЕРЕМЕННЫЕ
const float toDeg = 180.0 / M_PI;
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// инициализация
void initDMP() {
  devStatus = mpu.dmpInitialize();
  mpu.setDMPEnabled(true);
  mpuIntStatus = mpu.getIntStatus();
  packetSize = mpu.dmpGetFIFOPacketSize();
}

// получение углов в angleX, angleY, angleZ
void getAngles() {
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    angleX = ypr[2] * toDeg;
    angleY = ypr[1] * toDeg;
    angleZ = ypr[0] * toDeg;
  }

}
