#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient pusSubClient(wifiClient);

#define topicName "testing"

// Replace these with your SSID/Password
const char* ssid = "yourSSID";
const char* password = "yourPassword";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqttServer = "mqtt.flespi.io";
const int mqttPort = 1883; // non-ssl

// to connect secure port use WifiClientSecure instead of WiFiClient, see:
// https://github.com/knolleary/pubsubclient/issues/462#issuecomment-542911896
// and this:
// https://github.com/programmer131/ESP8266-gBridge-TLS/blob/master/esp8266_gbridgeTLS/esp8266_gbridgeTLS.ino
const char* mqttUser = "your_mqtt_token";
const char* mqttPassword = "";

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(WiFi.status());
    Serial.println(WL_CONNECTED);
    Serial.println("trying to connect to wifi...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pusSubClient.setServer(mqttServer, mqttPort);
  pusSubClient.setCallback(callback);
}

void loop() {

  if (!pusSubClient.connected()) {
    Serial.println("disconnected MQTT");
    reconnect();
  }
  pusSubClient.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!pusSubClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pusSubClient.connect(topicName, mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Subscribe
      pusSubClient.subscribe(topicName);
    }  else {
      Serial.print("failed, rc=");
      Serial.print(pusSubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  // Expected payload format
  /*
    {
    "topicName": "testing",
    "payload": {
      "sensor": "Distance",
      "time": 1351824120,
      "data": [48.75608,2.302038]
      }
    }
  */

  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  //parsing JSON
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  const char* sensor = doc["sensor"];
  long time          = doc["time"];
  double latitude    = doc["data"][0];
  double longitude   = doc["data"][1];

  Serial.print("Your sensor: ");
  Serial.print(sensor);

}
