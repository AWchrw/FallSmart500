#include <Mpu.h>
#include <MqttClient.h>
#include <cmath>

#define BUZZER 5
#define SW_PIN 2
#define FALL_THRESHOLD 300
#define ACCEL_BUFFER_SIZE 10

int fallThreshold = 300;

// MPU
MPU mpu;
float prevAccel, prevRotation;

// Buzzer
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerDuration = 500;

// MQTT
MqttClient client;
uint32_t last_publish;  
float accelBuffer[ACCEL_BUFFER_SIZE];
int counter;

bool sendAccel = true;

void triggerBuzzer() {
  digitalWrite(BUZZER, LOW);
  buzzerStartTime = millis();
  buzzerActive = true;
}

void onFallThresholdChanged(const char *message){
  fallThreshold = atoi(message);
}

void setup() {
  Serial.begin(115200);

  // MQTT Client
  client.connectWifi();
  client.connectMqtt(); 

  client.onTopic(TOPIC_FALL_THRESHOLD, onFallThresholdChanged);

  // MPU Setup
  mpu.init();

  // Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
}

void loop() {
  // Wifi & MQTT
  if (!client.wifiConnected) client.connectWifi();
  if (!client.mqttConnected) client.connectMqtt();
  client.mqttLoop();

  // Mpu
  Vector3 accel = mpu.readAccelerometer();
  Vector3 rotation = mpu.readRotation();
  bool isFalling = ( accel.magnitude >= FALL_THRESHOLD);

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

  bool isPressed = (digitalRead(SW_PIN) == LOW);

  // Turn off buzzer  
  if (buzzerActive && ( (millis() - buzzerStartTime >= buzzerDuration) || isPressed) ) {
    digitalWrite(BUZZER, HIGH);
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
  
  delay(200);
}