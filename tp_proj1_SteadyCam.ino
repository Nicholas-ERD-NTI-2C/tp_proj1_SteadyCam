// INCLUDES
#include <Wire.h>  // Wire library - used for I2C communication
#include <Servo.h> // the servo library

// SERVOS
Servo servoX; // the X axis servo
Servo servoY; // the Y axis servo 

// ACCELEROMETER
int ADXL345 = 0x53; // The ADXL345 sensor I2C address 

// CONST's
const int arraySize = 8;  // The size of the data aray

//FLOAT'S 
float X_out, Y_out, Z_out;  // Floats for storing the sensor data
float X_ang, Y_ang, Z_ang; // floats for storing the calculated angle
float sumY, sumX;   // variable for storing the angle sum   

// INT'S
int dataArrayX[arraySize];  // Define the array for the X axis angle values
int dataArrayY[arraySize];  // Define the array for the Y axis angle values
int currentIndex = 0;   // Variable to keep track of the current index


// SETUP
void setup() {

  // SERVOS
  servoX.attach(2); // initialize the servos, pin 2 & 3
  servoY.attach(3);

  // ACCELEROMETER  
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
  Wire.endTransmission();

  // INIT SERVO'S
  delay(10);
  updateServo(90, 90);
  delay(10);
}

void loop() {

  read_ang(); // run the read_ang function to get all updated variables

  // code for filling the sum with the array values
  sumY = 0;
  for (int i = 0; i < 8; i++) {
    sumY += dataArrayY[i]; 
  }
  sumX = 0;
  for (int i = 0; i < 8; i++) {
    sumX += dataArrayX[i];
  }
  
  // write the servos with the X and Y angles (the sum divided by amount of data points)
  updateServo(sumX / arraySize, sumY / arraySize); 
}

void read_ang() {  // the function for reading accelerometer data and updating the variables 
  // READING ACCELEROMETER DATA
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers

  // SETTING THE VARIABLE VALUES
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis value
  X_out = X_out / 256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis value
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis value
  Z_out = Z_out / 256;


  X_ang = atan(X_out / Z_out) * (180 / 3.1415);  // my shitty math for calculating the angle using the outputs and then writing it to the variables
  Y_ang = atan(Y_out / Z_out) * (180 / 3.1415);

  addToArray(); // add the y and X angles to their arrays
}

void addToArray() { // function for adding a item to the arrays, pushes out the oldest and adds the new value, keeping the array arraySize
  // Add the new value to the array
  dataArrayY[currentIndex] = Y_ang;
  dataArrayX[currentIndex] = X_ang;
  
  // Move to the next index
  currentIndex = (currentIndex + 1) % arraySize;
}

void updateServo(int x, int y) { // function for writing values to the arduino 
  if (Z_out > 0) {
    servoX.write(-x + 90);
    servoY.write(-y + 90);
  }
}
