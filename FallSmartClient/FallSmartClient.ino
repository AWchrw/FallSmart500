#include <Mpu.h>
#include <MqttClient.h>
#include <cmath>

#define BUZZER 41
#define FALL_THRESHOLD 0.3
#define ACCEL_BUFFER_SIZE 5

// MPU
MPU mpu;
float prevAccel, prevRotation;

// Buzzer
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerDuration = 10 * 1000;

// MQTT
MqttClient client;
uint32_t last_publish;

void triggerBuzzer() {
  digitalWrite(BUZZER, HIGH);
  buzzerStartTime = millis();
  buzzerActive = true;
}

void setup() {
  Serial.begin(115200);

  // MQTT Client
  client.connectWifi();
  client.connectMqtt(); 

  // MPU Setup
  mpu.init();

  // Buzzer
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // Wifi & MQTT
  if (!client.wifiConnected) client.connectWifi();
  if (!client.mqttConnected) client.connectMqtt();
  client.mqttLoop();

  // Mpu
  Vector3 accel = mpu.readAccelerometer();
  Vector3 rotation = mpu.readRotation();
  bool isFalling = ( fabs(accel.magnitude - prevAccel) / prevAccel ) >= FALL_THRESHOLD;

  Serial.printf("ACC %d %d %d %.2f  RT %d %d %d %.2f isFalling %d\n",
    accel.x, accel.y, accel.z, accel.magnitude,
    rotation.x, rotation.y, rotation.z, rotation.magnitude,
    isFalling);
    
  if (prevAccel > 0 && isFalling) {
    triggerBuzzer();
    if (client.mqttConnected) {
      String payload(isFalling);
      client.mqttPublish(TOPIC_FALL, payload.c_str());
    }
  }

  // Turn off buzzer  
  if (buzzerActive && millis() - buzzerStartTime >= buzzerDuration) {
    digitalWrite(BUZZER, LOW);
    buzzerActive = false;

    // It should send falling = false
    // This tied falling state with buzzer state
    if (client.mqttConnected && !isFalling) {
      String payload(isFalling);
      client.mqttPublish(TOPIC_FALL, payload.c_str());
    }
  }

  prevAccel = accel.magnitude;
  prevRotation = rotation.magnitude;
  
  delay(1000);
}