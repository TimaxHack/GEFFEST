#include<Wire.h>
#include "GyverButton.h"

#define BTN_W_PIN 5  
GButton btnW(BTN_W_PIN);

//Create thumb Sensors
int pinkie = 0; //Pinkie thumb
int finger = 0; //finger thumb


int pinkie_Data = A5;
int finger_Data = A6;


//const int MPU_addr = 0x68;
const int MPU2 = 0x69, MPU1 = 0x68;

//First MPU6050
int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;
int minVal = 265;
int maxVal = 402;
double x;
double y;
double z;

//Second MPU6050
int16_t AcX2, AcY2, AcZ2, Tmp2, GyX2, GyY2, GyZ2;
int minVal2 = 265;
int maxVal2 = 402;
double x2;
double y2;
double z2;

/*Autotune flex parameter
  For Debug Mode. Check the upper and lowe limit of the flex sensors
  3 Flex sensors used. Thumb, Middle, Pinkie
*/

int finger_high = 0;
int finger_low = 0;
int pinkie_high = 0;
int pinkie_low = 0;

//Stop Caliberating the Flex Sensor when complete
bool bool_caliberate = false;

//How often to send values to the Robotic Arm
int response_time = 100;


int flexposition;
int flexposition1;






int z_offset;
void setup() {
  btnW.setDebounce(90);        // настройка антидребезга (по умолчанию 80 мс)
  btnW.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  btnW.setClickTimeout(500);  // установка таймаута между кликами (по умолчанию 500 мс)  
  btnW.setTickMode(AUTO);
  pinMode(3, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(38400);
  delay(1000);

}

static uint32_t timer;
bool stringFlag = true;
int debugFlag = 0;
char data[30] = "";
void loop() {

  /*
    Note: Serial.print() would send all values the robotic arm using via bluetooth.
  */
  pinMode(3, HIGH); //Use basic LED as visual indicator if value being sent

  //debug_flex(); //Debug Mode on/off
  //Serial.println("test");
  //get values for first mpu having address of 0x68
  GetMpuValue1(MPU1);
  //Serial.prinlnt("  ");
  delay(10);

  //get values for second mpu having address of 0x69
 
  //Serial.println("");
  delay(10);

  //Print out a value, based on the change of the XYZ co-ordinates of 1st or 2nd MPU
  
  //Move Left
  pinkie = map(analogRead(pinkie_Data), 240, 420, 0, 100);
  finger = map(analogRead(finger_Data), 0, 500, 0, 100);
  //Serial.print(finger);
  //Serial.print('\t');
  //Serial.println(pinkie);
  


  //Calibrate to find upper and lower limit of the Flex Sensor
  if (bool_caliberate == false ) {
    delay(1000);

    finger_high = (finger * 1.03);
    finger_low = (finger * 0.8);

    pinkie_high = (pinkie * 1.06);
    pinkie_low = (pinkie * 0.8);

    bool_caliberate = true;
  }

  delay(response_time);


  
  if (btnW.isHold()){
  if ( x > 15 && x < 55) {
        Serial.print("R");
        delay(response_time);
      }
    
      //Move Right
      if ( x < 360 && x > 270) {
        Serial.print("r");
        delay(response_time);
      }
    
      //Claw Up
      if ( y > 20 && y < 80) {
        Serial.print("P");
        delay(response_time);
      }
    
      //  //Claw Down
      if ( y < 340 && y > 270) {
        Serial.print("p");
        delay(response_time);
      }  
      
      if (finger >= 80) {
        Serial.print("F");
        delay(response_time);
  
      }
      if (finger <= 60) {
        Serial.print("f");
        delay(response_time);
      }
      
      
      }
  else {
    if ( x > 15 && x < 55) {
      Serial.print("E");
      delay(response_time);
    }
  
    //Move Right
    if ( x < 360 && x > 270) {
      Serial.print("e");
      delay(response_time);
    }
  
    //Claw Up
    if ( y > 20 && y < 80) {
      Serial.print("S");
      delay(response_time);
    }
  
    //  //Claw Down
    if ( y < 340 && y > 270) {
      Serial.print("s");
      delay(response_time);
    }  
    
    if (finger >= 80) {
      Serial.print("B");
      delay(response_time);

    }
    if (finger <= 60) {
      Serial.print("b");
      delay(response_time);
    }
  }


  

  // read the values from Flex Sensors to Arduino
  

  // Pinkie
  


  // thumb 1 - thumb (Base Rotation)
  /*if (thumb >= thumb_high) {
    Serial.print("T");
    delay(response_time);
  }

  if (thumb <= thumb_low) {
    Serial.print("t");
    delay(response_time);
  }

  // finger 1 - Claw Bend/Open
  if (finger >= finger_high) {
    Serial.print("F");
    delay(response_time);
  }

  if (finger <= finger_low) {
    Serial.print("f");
    delay(response_time);
  }
  else {
    delay(5);
  }*/
 
}

void GetMpuValue1(const int MPU) {

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  AcX1 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1 = Wire.read() << 8 |  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1 = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Tmp1 = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  int xAng = map(AcX1, minVal, maxVal, -90, 90);
  int yAng = map(AcY1, minVal, maxVal, -90, 90);
  int zAng = map(AcZ1, minVal, maxVal, -90, 90);

  GyX1 = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1 = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1 = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  //-- Comment to Debug
    /*Serial.print("AngleX= ");
    Serial.print(x);
    Serial.print("\t");
  
    Serial.print("AngleY= ");
    Serial.print(y);
    Serial.print("\t");
  
    Serial.print("AngleZ= ");
    Serial.print(z);
    Serial.print("\t");
    Serial.println("-----------------------------------------");


    /*Serial.print("AcX = ");
    Serial.print(AcX1);
    Serial.print(" | AcY = ");
    Serial.print(AcY1);
    Serial.print(" | AcZ = ");
    Serial.print(AcZ1);
    Serial.print(" | GyX = ");
    Serial.print(GyX1);
    Serial.print(" | GyY = ");
    Serial.print(GyY1);
    Serial.print(" | GyZ = ");
    Serial.println(GyZ1);*/
}
