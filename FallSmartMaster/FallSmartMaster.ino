#include <MqttClient.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUZZER 41
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Buzzer
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerDuration = 10 * 1000;

// MQTT
MqttClient client;
uint32_t last_publish;

// ===== BUZZER =====
void triggerBuzzer() {
  digitalWrite(BUZZER, HIGH);
  buzzerStartTime = millis();
  buzzerActive = true;
}
void turnOffBuzzer() {
  digitalWrite(BUZZER, LOW);
  buzzerActive = false;
}

// ===== OLED =====
void displayInit() {
  Wire.begin(48, 47);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display(); 
}

void displayText(const char* text) {
  display.clearDisplay();
  display.setCursor(30, 30);  
  display.print(text);

  display.display();
}

// ===== MQTT TOPIC HANDLER =====
void onDebug(const char *message) {
  Serial.printf("%s\n", message);
}

void onFall(const char *message) {
  displayText(message);
  triggerBuzzer();
}

void onUnfall(const char *message) {
  displayText("Safe");
  turnOffBuzzer();
}

// Main
void setup() {
  Serial.begin(115200);

  // MQTT Client
  client.connectWifi();
  client.connectMqtt();
  client.onTopic(TOPIC_DEBUG, onDebug);
  client.onTopic(TOPIC_FALL, onFall);
  client.onTopic(TOPIC_UNFALL, onUnfall);

  // OLED
  displayInit();

  // Buzzer
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // Wifi & MQTT
  if (!client.wifiConnected) client.connectWifi();
  if (!client.mqttConnected) client.connectMqtt();
  client.mqttLoop();

  // Turn off buzzer  
  if (buzzerActive && millis() - buzzerStartTime >= buzzerDuration) {
    turnOffBuzzer();
  }

  delay(1000);
}