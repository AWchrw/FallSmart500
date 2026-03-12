#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <config.h>
#include <map>
#include <functional>

#define TOPIC_PREFIX "master"

// PUB
#define TOPIC_ACCELERATION TOPIC_PREFIX "/acceleration"
#define TOPIC_CONNECTION TOPIC_PREFIX "/connection"

// SUB
#define TOPIC_DEBUG TOPIC_PREFIX "/debug"
#define TOPIC_FALL TOPIC_PREFIX "/fall"
#define TOPIC_UNFALL TOPIC_PREFIX "/unfall"

class MqttClient {
  private:
    WiFiClient wifiClient;
    PubSubClient mqtt;
    std::map<String, std::function<void(const char*)>> topicHandlers;
    
    void onConnected();
    void mqttCallback(char* topic, byte* payload, unsigned int length);
  public:
    bool wifiConnected;
    bool mqttConnected;

    MqttClient();

    void connectWifi();
    void connectMqtt();
    void mqttPublish(const char *topic, const char *payload);
    void onTopic(const char *topic, std::function<void(const char*)> handler);
    void mqttLoop();
};

#endif