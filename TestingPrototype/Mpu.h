#ifndef MPU_H
#define MPU_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

struct Vector3 {
  int x;
  int y;
  int z;
  float magnitude;
};

class MPU {
  private:
    MPU6050 mpu;
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    float calculateMagnitude(int x, int y, int z);

  public:
    void init();
    Vector3 readAccelerometer();
    Vector3 readRotation();
};

#endif