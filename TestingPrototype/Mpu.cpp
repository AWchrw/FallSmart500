#include <Mpu.h>
float MPU::calculateMagnitude(int x, int y, int z) {
  return sqrt(x * x + y * y + z * z);
}

void MPU::init() {
  Serial.println("Initialize MPU");
  Wire.begin(21, 20);
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
}

Vector3 MPU::readAccelerometer() {
  Vector3 accel;
  mpu.getAcceleration(&ax, &ay, &az);
  accel.x = map(ax, -16384, 16384, 0, 180);
  accel.y = map(ay, -16384, 16384, 0, 180);
  accel.z = map(az, -16384, 16384, 0, 180);
  accel.magnitude = calculateMagnitude(accel.x, accel.y, accel.z);
  return accel;
}

Vector3 MPU::readRotation() {
  Vector3 rotation;
  mpu.getRotation(&gx, &gy, &gz);
  rotation.x = map(gx, -16384, 16384, -90, 90);
  rotation.y = map(gy, -16384, 16384, -90, 90);
  rotation.z = map(gz, -16384, 16384, -90, 90);
  rotation.magnitude = calculateMagnitude(rotation.x, rotation.y, rotation.z);
  return rotation;
} 