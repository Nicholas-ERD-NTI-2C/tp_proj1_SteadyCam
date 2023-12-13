/*
    Arduino and ADXL345 Accelerometer Tutorial
     by Dejan, https://howtomechatronics.com
*/

#include <Wire.h>  // Wire library - used for I2C communication
  #include <Servo.h>
  
  Servo servoX;
  Servo servoY;

int ADXL345 = 0x53; // The ADXL345 sensor I2C address

float X_out, Y_out, Z_out;  // Outputs
float X_ang, Y_ang, Z_ang;



const int arraySize = 8;  // Set the size of the array

int dataArrayX[arraySize];  // Define an array to store values
int dataArrayY[arraySize];  // Define an array to store values

int currentIndex = 0;      // Variable to keep track of the current index
float sumY, sumX;


void setup() {

  servoX.attach(2);
  servoY.attach(3);

  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
  Wire.endTransmission();
  delay(10);
  updateServo(90, 90);
  delay(10);
}

void loop() {
  Serial.print("bög");
  read_ang();

  sumY = 0;
  for (int i = 0; i < 8; i++) {
    sumY += dataArrayY[i];
  }

  sumX = 0;
  for (int i = 0; i < 8; i++) {
    sumX += dataArrayX[i];
  }


  Serial.println();
  Serial.print("X_ang= ");
  Serial.print(sumX / 8);
  Serial.print("   Y_ang= ");
  Serial.println(sumY / 8);
  Serial.print("   Az= ");
  Serial.println(Z_out);

  updateServo(sumX / 8, sumY / 8);
}

void read_ang() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis value
  X_out = X_out / 256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis value
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis value
  Z_out = Z_out / 256;

  X_ang = atan(X_out / Z_out) * (180 / 3.1415);
  Y_ang = atan(Y_out / Z_out) * (180 / 3.1415);

  addToArray();
}

void updateServo(int x, int y) {
  if (Z_out > 0) {
    servoX.write(-x + 90);
    servoY.write(-y + 90);
  }
}

void addToArray() {
  // Add the new value to the array
  dataArrayY[currentIndex] = Y_ang;
  dataArrayX[currentIndex] = X_ang;

  // Move to the next index
  currentIndex = (currentIndex + 1) % arraySize;
}
