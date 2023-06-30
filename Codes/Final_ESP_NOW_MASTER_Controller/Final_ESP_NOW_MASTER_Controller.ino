#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_AHTX0.h>
//For DB18S20 Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
// LoRA module
#include <SPI.h>
#include <LoRa.h>

//OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64
#define i2c_Address 0x3C 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


//LoRA module
#define ss 15
#define rst 13
#define dio0 12

//AHT21B sensor
Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;


//Soil Moisture sensor

const int soilMoistureSensorPin  = 32;

const int AirValue = 4095;
const int WaterValue = 2200;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;


//Rain Sensor
const int rainpin = 33;

int rainSensorValue;
String Rain_Status;

// DB18S20 Sensor

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int tempC;
// Relay pin

const int pumpRelayPin = 16;

//Global Variables

int board1_moistureVal,board2_moistureVal;
int board1_pumpStatus,board2_pumpStatus;
int numSlaves,numPeers;
esp_now_peer_info_t peerInfo;

String a,b;
//FOR ESP_NOW connection

typedef struct struct_message {
  int id;
  int soilMoistureVal;
  int pumpStatus;
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};
void setup(){
  PinMode();
  // put your setup code here, to run once:
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //INIT Display
  InitDisplay();
  
  //Intro Display
  IntroDisplay();

  
   //Init ESP-NOW
  InitEsp_NOW();


  // Print the number of connected peers
  Serial.print("Number of connected peers: ");
  Serial.println(numPeers);
  
   // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv); 

  //INIT AHT sensor
  InitAHTSensor();

  // INIT LoRA
  InitLoRA();



}

void loop(){
  readData();
  ahtSensorRead();
  soilMoistureRead();
  rainSensorRead();
  DB18S20SensorRead();
  LoRAPacketSender();
  sensorDataDisplay(); 
}



void ahtSensorRead(){
  aht.getEvent(&humidity, &temp);
}

void soilMoistureRead(void)
{
  soilMoistureValue = analogRead(soilMoistureSensorPin );
  if (soilMoistureValue > AirValue)
  {
    soilMoisturePercent = 0;
  }
  else if (soilMoistureValue < WaterValue)
  {
    soilMoisturePercent = 100;
  }
  else
  {
  soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  }
  
}
void rainSensorRead(void)
{
    
  rainSensorValue = analogRead(rainpin);
  //Serial.println(rainSensorValue);
  switch (rainSensorValue) {
    case 4095:
      Rain_Status = "No rain";
      break;
    case 1 ... 511:
      Rain_Status = "Extreme";
      break;
    case 512 ... 1023:
      Rain_Status = "Very heavy";
      break;
    case 1024 ... 2047:
      Rain_Status = "Heavy";
      break;
    case 2048 ... 3071:
      Rain_Status = "Moderate";
      break;
    case 3072 ... 4094:
      Rain_Status = "Light";
      break;
  }
}

void DB18S20SensorRead(){
  // Request temperature to all devices on the data line
  sensors.requestTemperatures(); 
  tempC=sensors.getTempCByIndex(0);
}

void readData(){
  board1_moistureVal = boardsStruct[0].soilMoistureVal;
  board1_pumpStatus = boardsStruct[0].pumpStatus;
  board2_moistureVal = boardsStruct[1].soilMoistureVal;
  board2_pumpStatus = boardsStruct[1].pumpStatus;
  Serial.println("Board Value Check");
  Serial.println(board1_moistureVal);
  Serial.println(board1_pumpStatus);
  Serial.println(board2_moistureVal);
  Serial.println(board2_pumpStatus);
}

void InitLoRA(){
  // LoRA module
 while (!Serial);
  Serial.println("LoRa Sender");
 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module           

  
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4,30);
  display.print("LoRA Initialised");
  display.display();
  delay(1000);
}

void PinMode(){
  pinMode(soilMoistureSensorPin, INPUT);
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(rainpin, INPUT); 
}

void InitAHTSensor(){
    if (!(aht.begin(&Wire,0,0x38))) {
    Serial.println("Could not find a valid sensor, check wiring!");
    while (1);
  }
}

void InitDisplay(){
  if(!display.begin(i2c_Address, true)) {
  Serial.println(F("SH1106 allocation failed"));
  for(;;);
  }
}

void InitEsp_NOW(){
  //Init ESP-NOW
  if (esp_now_init() == ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(4,30);
  display.print("ESP_NOW Started");
  display.display();
  delay(1000);
  return;
  }  
}


void IntroDisplay(){
 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(SH110X_WHITE);
 display.setCursor(3,5);
 display.println("   Soil & ");
 display.println("  Weather ");
 display.println("  Monitor ");
 display.display();
 delay(1000);

 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(SH110X_WHITE);
 display.setCursor(0,1);
 display.println("Developed  by  ");
 display.println("   IIT  ");
 display.println(" ROORKEE");
 display.display();
 delay(1000);

 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(SH110X_WHITE);
 display.setCursor(5,20);
 display.println("   Wait...  ");
 display.display();
 delay(3000);
}

void LoRAPacketSender(){
  LoRa.beginPacket();
  //Soil Moiture Data
  LoRa.print(soilMoisturePercent);
  LoRa.print(":");
  if (soilMoisturePercent < 35) {
  LoRa.print("ON");
  LoRa.print(":");
  }
  else {
  LoRa.print("OFF");
  LoRa.print(":");
  }
  //Rain Sensor
  LoRa.print(Rain_Status);
  LoRa.print(":");
  //Ambient Temperature
  LoRa.print(String(temp.temperature));
  LoRa.print(":");
  //Ambidient hummidity
  LoRa.print(String(humidity.relative_humidity));
  LoRa.print(":");
  //Soil temperature
  LoRa.print(tempC);
  LoRa.print(":");
  //NODE_1 data
  LoRa.print(board1_moistureVal);
  LoRa.print(":");
  LoRa.print(board1_pumpStatus);
  LoRa.print(":");
  //NODE_2 data
  LoRa.print(board2_moistureVal);
  LoRa.print(":");
  LoRa.print(board2_pumpStatus);
  LoRa.endPacket(); 
}

void sensorDataDisplay(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(3,10);
  display.print("Central Node Data: ");
  display.print(" ");
  display.setCursor(0,30);
  display.print("Temperature: ");
  display.print(String(temp.temperature));
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.print("C");
  
  // display humidity
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 50);
  display.print("Humidity: ");
  display.print(String(humidity.relative_humidity));
  display.print(" %");
  display.display();
  delay(2000); 

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(5,0);
  display.print("   Soil Data  ");  
  display.setCursor(0,20);
  display.print("Moisture: ");
  display.print(soilMoisturePercent);
  display.print(" %"); 

  // if the soil moisture level is below 35%, turn the pump on:
  if (soilMoisturePercent < 35) {
  digitalWrite(pumpRelayPin, HIGH);
  display.setCursor(0,35);
  display.print("PUMP Status: ");
  display.print("ON");
  }
  
  // if the soil moisture level is above 75%, turn the pump off:
  if (soilMoisturePercent > 75) {
  digitalWrite(pumpRelayPin, LOW);
  display.setCursor(0,45);
  display.print("PUMP Status: ");
  display.print("OFF");
  }
  


  display.setCursor(0,55);
  display.print("Soil Temperature: ");
  display.print(tempC);
  display.cp437(true);
  display.write(167);
  display.print("C"); 
  display.display();
  delay(2000);
 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(5,20);
  display.print("Rain Status: ");
  display.println(Rain_Status);
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10,0);
  display.print("   Node 1 Data  ");  
  display.setCursor(10,35);
  display.print("Moisture: ");
  display.print(board1_moistureVal);
  display.print(" %"); 

  // if the soil moisture level is below 35%, turn the pump on:
  if (board1_pumpStatus == 1) {
  display.setCursor(10,45);
  display.print("PUMP Status: ");
  display.print("ON");
  }
  Serial.println(board1_pumpStatus);
  
  // if the soil moisture level is above 75%, turn the pump off:
  if (board1_pumpStatus == 0) {
  display.setCursor(10,45);
  display.print("PUMP Status: ");
  display.print("OFF");
  }
  display.display();
  delay(4000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10,0);
  display.print("   Node 2 Data  ");  
  display.setCursor(10,35);
  display.print("Moisture: ");
  display.print(board2_moistureVal);
  display.print(" %"); 

  // if the soil moisture level is below 35%, turn the pump on:
  if (board2_pumpStatus == 1) {
  display.setCursor(10,45);
  display.print("PUMP Status: ");
  display.print("ON");
  }
  
  // if the soil moisture level is above 75%, turn the pump off:
  if (board2_pumpStatus == 0) {
  display.setCursor(10,45);
  display.print("PUMP Status: ");
  display.print("OFF");
  } 
  display.display();
  delay(4000);
}

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].soilMoistureVal = myData.soilMoistureVal;
  boardsStruct[myData.id-1].pumpStatus = myData.pumpStatus;
  Serial.printf("Soil_Moisture_Value: %d \n", boardsStruct[myData.id-1].soilMoistureVal);
  Serial.printf("Pump_Status: %d \n", boardsStruct[myData.id-1].pumpStatus);
  Serial.println();
}
