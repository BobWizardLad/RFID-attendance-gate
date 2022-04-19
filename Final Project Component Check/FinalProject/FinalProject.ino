#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define RED_LED 1
#define GRN_LED 0

void greenOn(); // Green LED ON
void greenOff(); // Green LED OFF
void redOn(); // Red LED ON
void redOff(); // Red LED OFF

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

MFRC522DriverPinSimple ss_pin(11); // Configurable, see typical pin layout above.

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
//MFRC522DriverI2C driver{}; // Create I2C driver.
MFRC522 mfrc522{driver};  // Create MFRC522 instance.

void setup() {
  // put your setup code here, to run once:
  // Red LED Output
  pinMode(RED_LED, OUTPUT);
  // Green LED Output 
  pinMode(GRN_LED, OUTPUT);
  //  Serial.begin(9600);
  //  Keyboard.begin();
  //  SPI.begin();
  //  mfrc522.PCD_Init();
    Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();  // Init MFRC522 board.
  
  Serial.println(F("*****************************"));
  Serial.println(F("MFRC522 Digital self test"));
  Serial.println(F("*****************************"));
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Show version of PCD - MFRC522 Card Reader.
  Serial.println(F("-----------------------------"));
  Serial.println(F("Only known versions supported"));
  Serial.println(F("-----------------------------"));
  Serial.println(F("Performing test..."));
  bool result = mfrc522.PCD_PerformSelfTest(); // Perform the test.
  Serial.println(F("-----------------------------"));
  Serial.print(F("Result: "));
  if (result)
    Serial.println(F("OK"));
  else
    Serial.println(F("DEFECT or UNKNOWN"));
  Serial.println();

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display

  oled.setTextSize(1);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 10);        // position to display
  oled.println("Student ID: \n    019203901293"); // text to display
  oled.display();   
}


void loop() {
  // put your main code here, to run repeatedly:
  while(1){
    redOn();
    greenOff();
    delay(1000);
    greenOn();
    redOff();
    delay(1000);

  }
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
