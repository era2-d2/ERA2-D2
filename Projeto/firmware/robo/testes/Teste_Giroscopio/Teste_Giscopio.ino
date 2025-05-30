#include <Wire.h>
const int MPU = 0x68; // MPU6050 I2C address
float AccZ;
float GyroZ;
float gyroAngleZ, yaw;
float GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

void setup() {
  Serial.begin(19200);
  Wire.begin();                      // Initialize communication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        // End the transmission
  // Call this function to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
}

void loop() {
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000.0; // Divide by 1000 to get seconds
  
  // === Read gyroscope data (Z-axis only) === //
  Wire.beginTransmission(MPU);
  Wire.write(0x43 + 4); // Gyro Z-axis data register address 0x47
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 2, true); // Read 2 registers total for Z-axis
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range, divide by 131.0

  // Correct the output with the calculated error value
  GyroZ = GyroZ + 0.79; // GyroErrorZ ~ (-0.79)
  
  // Calculate yaw (angle) from gyro data
  yaw = yaw + GyroZ * elapsedTime; // deg/s * s = deg
  
  // Print the yaw value on the serial monitor
  Serial.println(yaw);
}

void calculate_IMU_error() {
  // Call this function in the setup section to calculate the gyro Z-axis error.
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x47); // Gyro Z-axis data register address
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 2, true); // Read 2 registers total for Z-axis
    GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
    // Sum all readings
    GyroErrorZ = GyroErrorZ + GyroZ;
    c++;
  }
  // Divide the sum by 200 to get the error value
  GyroErrorZ = GyroErrorZ / 200;
  
  // Print the error value on the Serial Monitor
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}
