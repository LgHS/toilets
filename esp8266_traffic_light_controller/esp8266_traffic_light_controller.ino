/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/
#include "ssid.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(0, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    if(strcmp(topic, "/lghs/traffic_light/green") == 0) {
      Serial.println("sending LOW on 2 (green)");
      digitalWrite(2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    }
    if(strcmp(topic, "/lghs/traffic_light/red") == 0) {
      Serial.println("sending LOW on 0 (red)");
      digitalWrite(0, LOW);   // Turn the LED on (Note that LOW is the voltage level
    }
  } else {
    if(strcmp(topic, "/lghs/traffic_light/green") == 0) {
      Serial.println("sending HIGH on 2 (green)");
      digitalWrite(2, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    }
    if(strcmp(topic, "/lghs/traffic_light/red") == 0) {
      Serial.println("sending HIGH on 0 (red)");
      digitalWrite(0, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("traffic_light")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("/lghs/traffic_light/red");
      client.subscribe("/lghs/traffic_light/green");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
