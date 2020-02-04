// LED Display Libs
#include "SPI.h"
#include "Wire.h"
#include "stdio.h" 
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Adafruit_SSD1306.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDER 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

// Gas Sensor & LED Alert Pins
#define gasSensor 34
#define alertLED 23
#define button 32
#define test 33
#define buzzer 14

int alertValue = 0;
 
// Wifi and IoT Libs and Values
// * Hidden for security reasons
#include "WiFi.h"
#include "WiFiClientSecure.h"
#define WIFI_SSID "NETGEAR50" 
#define WIFI_PASS "aquaticpanda498"
#define API_KEY "dkmxnPR2PPNL6Spb1sNZxP"
#define SERVER "maker.ifttt.com"
#define PORT 443


void setup() {
  int i = 10; 
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDER);
  pinMode(alertLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(test, INPUT_PULLUP);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED && i >=0) {
    delay(1000);
    Serial.print(i);
    Serial.print(", ");
    i--;
  }
}

void loop() {
 
  int gasLevel = analogRead(gasSensor); // Reads sensor value
  gasLevel = gasLevel - 1300;
  Serial.println(gasLevel);
  // Displays on I2C LED Display
  display.clearDisplay(); // Has to clear the display 
  display.setTextSize(1);
  display.setTextColor(WHITE);`````
  display.setFont(&FreeSans9pt7b);``````````````
  display.setCursor(20,12);
  display.println("CO Level: ");`
  display.setTextSize(2);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(30,45);
  display.println(gasLevel);
  display.display();
  while(alertValue == 1 && digitalRead(button) != 0){
    triggerAlarm(gasLevel);
  }
  if (gasLevel > 150){ // Alert System
    alertValue = 1;
    triggerAlarm(gasLevel);
  }
  else{
    alertValue = 0;
    delay(1000);
    digitalWrite(alertLED, LOW);
  }
}

void triggerAlarm(int gasLevel){
  sendAlert(gasLevel);
  
  for (int x = 0;x<5;x++){
    digitalWrite(alertLED, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(alertLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
}

void sendAlert(int gasLevel) {
  // Creates a WiFi client
  WiFiClientSecure client; 
  if (!client.connect(SERVER, PORT)) {
    Serial.println("Connection failed");
    return;
  }
  // URL for the trigger
  String event = "smokeAlert";
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += API_KEY;
  url += "?value1=";
  url += String(gasLevel);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // Sends a request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + SERVER + "\r\n" + 
               "Connection: close\r\n\r\n");
  // Client ends
  client.stop();
}
