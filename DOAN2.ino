
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ADS1015.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h> 
#include <ThingSpeak.h> 
unsigned int t ;
float n ;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define ONE_WIRE_BUS 25  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature nhietdo(&oneWire); 
DFRobot_ESP_EC ec;
Adafruit_ADS1115 ads;

float voltage,ecValue,temperature = 25 ; 

// Connect internet 
const char* ssid = "Qq";
const char* pass = "q212002*";

unsigned long myChannelNumber = 1;  // Replace the 0 with your channel number, channel của bạn
const char * myWriteAPIKey = "J3WGYVUSI4QQJMEY "; 

WiFiClient wifiClient; 
//PubSubClient client(wifiClient);


void setup(){
  Serial.begin(115200);
  EEPROM.begin(32);
  ec.begin();
  nhietdo.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(1000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  ThingSpeak.begin(wifiClient);
}  

// hien thi OLED 
 void displayData(float a , float b){
    display.setTextSize(2);
    display.setTextColor(WHITE);
  
    display.setCursor(0, 10);
    display.print("T:");
    display.print(a, 2);
    display.drawCircle(85, 10, 2, WHITE); // put degree symbol ( ° )
    display.setCursor(90, 10);
    display.print("C");
  
    display.setCursor(0, 40);
    display.print("EC:");
    display.print(b, 2);
    display.display();
    display.clearDisplay();
}

void Temperature_Ec(){

  if(millis()-t>1){
    voltage = analogRead(A0) ;// doc dien ap chan gpio 36 
    nhietdo.requestTemperatures();
    temperature = nhietdo.getTempCByIndex(0)  ; // doc cam bien nhiet do de thuc hien bu 
    ecValue = ec.readEC(voltage,temperature); // chuyen doi dien ap sang EC co bu  nhiet 

    Serial.print("Temperature:");
    Serial.print(temperature, 2);
    Serial.println("ºC");
  
    Serial.print("EC:");
    Serial.println(ecValue, 2);
    ec.calibration(voltage, temperature);

    displayData(temperature,ecValue);
    t = millis();
  }
  //doc du lieu len 
  int x =  ThingSpeak.setField(1, temperature ); //setField(field, value)
  int y =  ThingSpeak.setField(2, ecValue); //setField(field, value)
  int z = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (z == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(z));
  }
}

void loop(){ 
  Temperature_Ec();
}