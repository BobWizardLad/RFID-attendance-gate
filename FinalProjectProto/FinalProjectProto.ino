#include <WiFi101.h>
#include <PubSubClient.h>
#include "credentials.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>
#include <algorithm>

#define SS_PIN 11
#define RST_PIN 6
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

// Define MQTT topics
#define TOPIC "uark/csce5013/kaorman/attendance"
#define USERNAME "kaorman"
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define RED_LED 1
#define GRN_LED 0

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Set SSID and network password
char ssid[] = SSID;
char password[] = PASSKEY;

// char studentID;

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
char IDarray[18];
char Namearray[18];

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
void stuID(); // Reads card block buffer to ID char array
void stuName(); // Reads card block buffer to Name char array
void sendTask(); // Posts student info to MQTT

void setup() {
  // put your setup code here, to run once:

  // Serial connection set to 115200 BAUD
  Serial.begin(115200);

  SPI.begin(); // Init SPI bus for rfid
  mfrc522.PCD_Init(); // Init MFRC522 card reader

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
      //lcdTask();
    break;
    // Sending data to MQTT
    case SEND:
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
      sendTask();
      lcdTask();
      curMode = RECORD;
    break;
  }
}

void lcdTask() {

}

void rfidTask(){
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Student Detected:**"));

  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

Serial.print(F("ID: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET ID
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT ID
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");
  stuID(buffer1, sizeof(buffer1));
  // for(int i = 0; i < strlen(IDarray); i++){
  //   Serial.print(IDarray[i]);
  // }

  //---------------------------------------- GET LAST NAME

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println();

  Serial.print(F("Last Name: "));

  //PRINT LAST NAME
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }

  stuName(buffer2, sizeof(buffer2));
  
  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  Serial.println();
  Serial.println();
  curMode = SEND;
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

void stuID(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
      if(buffer[i] != ' '){
        IDarray[i]=char(buffer[i]);
      }
      else{
        break;
      }
    }
}

void stuName(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
      if(buffer[i] != ' '){
        Namearray[i]=char(buffer[i]);
      }
      else{
        break;
      }
    }
}

void sendTask(){
  //String s = String(IDarray);
  char sendBuffer[100];
  *std::remove(IDarray, IDarray+strlen(IDarray), '\n') = '\0';
  *std::remove(Namearray, Namearray+strlen(Namearray), '\n') = '\0';
  strcat(sendBuffer, "Student ID: ");
  strcat(sendBuffer, IDarray);
  strcat(sendBuffer, " Student Name: ");
  strcat(sendBuffer, Namearray);
  *std::remove(sendBuffer, sendBuffer+strlen(sendBuffer), '\n') = '\0';
  Serial.print("Sending attendance record...");
  Serial.println();
  client.publish(TOPIC, sendBuffer);
  memcpy(sendBuffer, NULL, strlen(sendBuffer));
}
