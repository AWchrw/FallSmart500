#include <MqttClient.h>

MqttClient::MqttClient()
  : mqtt(MQTT_BROKER, 1883, wifiClient)
{
    wifiConnected = false;
    mqttConnected = false;
}

void MqttClient::onConnected() {
  String connected(mqttConnected);
  mqttPublish(TOPIC_CONNECTION, connected.c_str());
}

void MqttClient::mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0'; 

  Serial.printf("Received TOPIC: %s with msg %s\n", topic, message);

  String topicStr(topic);

  auto it = topicHandlers.find(topicStr);
  if (it != topicHandlers.end()) {
    it -> second(message);
  }
}

void MqttClient::connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
  Serial.printf("Connecting to WiFi %s.\n", WIFI_SSID);

  if (!WiFi.status() == WL_CONNECTED) {
    Serial.println("Connection failed retrying...");
    return;
  }

  Serial.println("WiFi connected.");
  wifiConnected = true;
}

void MqttClient::connectMqtt() {
  Serial.printf("Connecting to MQTT broker at %s\n", MQTT_BROKER);
  if (!mqtt.connect("")) {
    Serial.println("Failed to connect to MQTT broker.");
    return;
  }

  mqtt.setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->mqttCallback(topic, payload, length);
  });
  
  Serial.println("MQTT broker connected.");
  mqttConnected = true;

  if (!topicHandlers.empty()) {
    for (auto &pair : topicHandlers) {
      mqtt.subscribe(pair.first.c_str());
    }
  }

  onConnected();
}

void MqttClient::mqttPublish(const char *topic, const char *payload) {
  Serial.printf("Sent %s with %s\n", topic, payload);
  bool res = mqtt.publish(topic, payload);
  if (!res) Serial.println("Failed to sent!");
}

void MqttClient::onTopic(const char* topic, std::function<void(const char*)> handler) {
  topicHandlers[String(topic)] = handler;
  mqtt.subscribe(topic);
}

void MqttClient::mqttLoop() {
  mqtt.loop();
}