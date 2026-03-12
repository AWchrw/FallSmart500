  #include <Mpu.h>
#include <MqttClient.h>
#include <cmath>

#define BUZZER 5
#define SW_PIN 2
#define FALL_THRESHOLD_PERCENTAGE 0.3
#define FALL_THRESHOLD 280

// MPU
MPU mpu;
float prevAccel, prevRotation;

// Buzzer
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerDuration = 7 * 1000;

// Fall Buffer
uint32_t lastFallSent;


// MQTT
MqttClient client;
uint32_t lastHeartbeatPublish;

void triggerBuzzer() {
  digitalWrite(BUZZER, LOW);
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

  pinMode(SW_PIN, INPUT_PULLUP);

  // Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
}

void loop() {
  // Wifi & MQTT
  if (!client.wifiConnected) client.connectWifi();
  if (!client.mqttConnected) client.connectMqtt();
  client.mqttLoop();

  uint32_t now = millis();

  // Mpu
  Vector3 accel = mpu.readAccelerometer();
  Vector3 rotation = mpu.readRotation();
  bool isFalling = ( accel.magnitude >= FALL_THRESHOLD);
  // bool isFalling = ( fabs(accel.magnitude - prevAccel) / prevAccel >= FALL_THRESHOLD_PERCENTAGE);
  bool fallingBuffer = now - lastFallSent <= 7 * 1000;

  Serial.printf("ACC %d %d %d %.2f  RT %d %d %d %.2f isFalling %d\n",
    accel.x, accel.y, accel.z, accel.magnitude,
    rotation.x, rotation.y, rotation.z, rotation.magnitude,
    isFalling);
    
  if (prevAccel > 0 && isFalling && !fallingBuffer) {
    triggerBuzzer();
    lastFallSent = now;
    if (client.mqttConnected) {
      String payload(isFalling);
      client.mqttPublish(TOPIC_FALL, payload.c_str());
    }
  }

  bool isPressed = (digitalRead(SW_PIN) == LOW);
  bool buzzerExpire = (now - buzzerStartTime) >= buzzerDuration;

  // Turn off buzzer  
  if (buzzerActive && isPressed ) {
    digitalWrite(BUZZER, HIGH);
    buzzerActive = false;
    Serial.printf("%d", isPressed);

    // It should send falling = false
    // This tied falling state with buzzer state
    if (client.mqttConnected) {
      String payload(0);
      client.mqttPublish(TOPIC_FALL, payload.c_str());
    }
  }
  prevAccel = accel.magnitude;
  prevRotation = rotation.magnitude;

  if (millis() - lastHeartbeatPublish >= 50 * 1000) {
    String payload(now);
    client.mqttPublish(TOPIC_HEARTBEAT, payload.c_str());
    lastHeartbeatPublish = now;
  }
  
  delay(100);
}