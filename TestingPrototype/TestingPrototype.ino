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
const unsigned long buzzerDuration = 300;

// MQTT
MqttClient client;
uint32_t last_publish;
float accelBuffer[ACCEL_BUFFER_SIZE];
int counter;

bool sendAccel = false;

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
    if (client.mqttConnected) {
      String payload(isFalling);
      client.mqttPublish(TOPIC_FALL, payload.c_str());
    }
  }

  prevAccel = accel.magnitude;
  prevRotation = rotation.magnitude;

  // Buzzer
  // digitalWrite(BUZZER, HIGH);

  if (client.mqttConnected && sendAccel) {
    accelBuffer[counter] = accel.magnitude;
    counter += 1;

    if (counter == ACCEL_BUFFER_SIZE) {
      char payload[200];
      payload[0] = '\0';
      for (int i = 0; i < ACCEL_BUFFER_SIZE; i++) {
        char temp[20];
        snprintf(temp, sizeof(temp), "%.2f", accelBuffer[i]);
        strcat(payload, temp);

        if (i < ACCEL_BUFFER_SIZE - 1) {
          strcat(payload, ",");
        }
      }

      counter = 0;
      client.mqttPublish(TOPIC_ACCELERATION, payload);
    }
  }
  
  delay(1000);
}