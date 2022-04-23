#include <WiFi101.h>
#include <PubSubClient.h>
#include "credentials.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define MQTT topics
#define TOPIC "attendance"
#define USERNAME "kaorman"
#define SERVO_PIN 9
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define RED_LED 1
#define GRN_LED 0

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Set SSID and network password
char ssid[] = SSID;
char password[] = PASSKEY;

//Status
int status = WL_IDLE_STATUS;
const char* mqtt_server = "broker.hivemq.com";
int led1_status = 0;

// Initialize PubSubClient 
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Declare global variables and arrays
long lastMsg = 0;
char msg[100];
int attMessageArrived = 0;
char attBuffer[100];

// Create states of operation
typedef enum mode{START, RECORD, SEND} MODE;
// Create topic states
typedef enum topic{noTopic, attTopic} TOPIC_SEL;

// Initialize state to START
MODE curMode = START;

// Initialize topic to noTopic
TOPIC_SEL curTopic = noTopic;

// Function prototypes
void lcdTask(); // Handles lcd display features
void rfidTask(); // Handles rfid reading 
void greenOn(); // Green LED ON
void greenOff(); // Green LED OFF
void redOn(); // Red LED ON
void redOff(); // Red LED OFF

void setup() {
  // put your setup code here, to run once:
  // Serial connection set to 115200 BAUD
  Serial.begin(115200);
  // Connect to local wifi
  setup_wifi();
  // Initial connection to mqtt server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Start state in reset state
  curMode = START;
}

void setup_wifi() {
  delay(1000);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, password);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback handles mqtt monitoring
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial Monitor output showing massage arrival and payload
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for(int i=0; i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  // Sets current topic based on what has been posted to mqtt
  if(strncmp(topic, TOPIC, strlen(TOPIC) == 0)){
    curTopic = attTopic;
  }
  else {
    curTopic = noTopic;
  }

  // Switch statement to handle what to do when a message is recieved in a current topic
  switch(curTopic) {
    case noTopic:
      Serial.println("Error: message is from an unknown topic.");
      return;
    break;
    case attTopic:
      //Copy payload into angle buffer after clearing it
      //memcpy(attBuffer,NULL,length+1);
      //memcpy(attBuffer,payload,length);
      attMessageArrived = 0;
      return;
    break;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //Attempt to connect
    if (client.connect(USERNAME)) {
      Serial.println("connected");
      // Once connected, subscribe to input channels
      //client.subscribe(TOPIC);
      client.subscribe(TOPIC);
  } else {
      Serial.print("failed, rc =");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(curMode) {
    // Start state with lcd reading "BROKEN"
    case START:
      curMode = RECORD;
    break;
    // Recieving RFID attendance operation
    case RECORD:
      rfidTask();
      lcdTask();
    break;
    // Sending data to MQTT
    case SEND:
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
      lcdTask();
      curMode = RECORD;
    break;
  }
}

void lcdTask() {

}

void rfidTask(){

}

void greenOn(){
  digitalWrite(GRN_LED, HIGH);
}

void greenOff(){
  digitalWrite(GRN_LED, LOW);
}

void redOn(){
  digitalWrite(RED_LED, HIGH);
}

void redOff(){
  digitalWrite(RED_LED, LOW);
}