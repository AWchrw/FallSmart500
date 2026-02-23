#include "Wire.h"
#include "MPU6050.h"

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx , valy , valz;

struct Vector3 {
  int x;
  int y;
  int z;
};

Vector3 accel;
Vector3 gyro;

void readMpu() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  accel.x = map(ax, -16384, 16384, 0, 180);
  accel.y = map(ay, -16384, 16384, 0, 180);
  accel.z = map(az, -16384, 16384, 0, 180);

  gyro.x = map(ax, -16384, 16384, -90, 90);
  gyro.y = map(ay, -16384, 16384, -90, 90);
  gyro.z = map(az, -16384, 16384, -90, 90);  
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin(21, 20);
  Serial.begin(115200);
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
}

void loop() {
  readMpu();
  Serial.printf("ACC %d %d %d  GYR %d %d %d\n",
    accel.x, accel.y, accel.z,
    gyro.x, gyro.y, gyro.z);
  delay(100);
}
