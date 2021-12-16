#include <WiFi.h>
#include <PubSubClient.h>
WiFiClient wifiClient;
PubSubClient pusSubClient(wifiClient);

#define yourTopicName "testing-topic"

// Replace these with your SSID/Password
const char* ssid = "your SSID";
const char* password = "you Password";


// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqttServer = "mqtt.flespi.io";
const int mqttPort = 1883; // non-ssl

// to connect secure port use WifiClientSecure instead of WiFiClient, see:
// https://github.com/knolleary/pubsubclient/issues/462#issuecomment-542911896
// and this:
// https://github.com/programmer131/ESP8266-gBridge-TLS/blob/master/esp8266_gbridgeTLS/esp8266_gbridgeTLS.ino
const char* mqttUser = "your mqtt token";
const char* mqttPassword = "";

long lastMsg = 0;
char msg[50];
int value = 0;

// LED Pin
const int ledPin = 4 ;

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

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

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
    if (pusSubClient.connect(yourTopicName, mqttUser, mqttPassword)) {
      Serial.println("connected");

      // Subscribe
      pusSubClient.subscribe(yourTopicName);
      if (pusSubClient.connect(yourTopicName, mqttUser, mqttPassword)) {
        Serial.println("connected");
      }
    }  else {
      Serial.print("failed, rc=");
      Serial.print(pusSubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == yourTopicName) {
    Serial.print("Topic arrived on yourTopicName");
    Serial.print("Changing output to ");
    if (messageTemp == "hello") {
      Serial.println("hello");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "world") {
      Serial.println("world");
      digitalWrite(ledPin, LOW);
    }
  }

}
