#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>

// -- [Config] --
#define WIFI_SSID "NavigatorNew_2.4G"
#define WIFI_PASS "0811785733"

#define MQTT_BROKER "192.168.1.40"
// ----

#define TOPIC_PREFIX "client01"

// PUB
#define TOPIC_ACCELERATION TOPIC_PREFIX "/acceleration"
#define TOPIC_CONNECTION TOPIC_PREFIX "/connection"
#define TOPIC_FALL TOPIC_PREFIX "/fall"

// SUB
#define TOPIC_DEBUG TOPIC_PREFIX "/debug"

class MqttClient {
  private:
    WiFiClient wifiClient;
    PubSubClient mqtt;
    
    void onConnected();
    void onDebugTopic(const char *payload);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
  public:
    bool wifiConnected;
    bool mqttConnected;

    MqttClient();

    void connectWifi();
    void connectMqtt();
    void mqttPublish(const char *topic, const char *payload);
    void mqttLoop();
};

#endif