#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xEC, 0x94, 0xCB, 0x6D, 0x67, 0xFC};

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 1

// Structure to send data
typedef struct {
int id;
int soilMoisturePercent;
int pumpStatus;
} sensor_data;

//Soil Moisture sensor

const int soilMoistureSensorPin  = A0;

const int AirValue = 4095;
const int WaterValue = 2200;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

const int pumpRelayPin = D1;

// Create a sensor_data struct to store variables to be sent
sensor_data myData;


void soilMoisture(void)
{
  soilMoistureValue = analogRead(soilMoistureSensorPin);
  Serial.println("Soil Moisture: ");
  Serial.print(soilMoistureValue);  
  if (soilMoistureValue > AirValue)
  {
    soilMoisturePercent = 0;
    Serial.println("Soil Moisture Percentage: ");
    Serial.print(soilMoisturePercent);
  }
  else if (soilMoistureValue < WaterValue)
  {
    soilMoisturePercent = 100;
    Serial.println("Soil Moisture Percentage: ");
    Serial.print(soilMoisturePercent);
  }
  else
  {
  soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  Serial.println("Soil Moisture Percentage: ");
  Serial.print(soilMoisturePercent);
  }
  
}

void setup() {
// Initialize Serial Monitor
Serial.begin(115200);
pinMode(pumpRelayPin, OUTPUT);

// Set device as a Wi-Fi Station
WiFi.mode(WIFI_STA);
WiFi.disconnect();

// Initialize ESP-NOW
if (esp_now_init() != 0) {
Serial.println("Error initializing ESP-NOW");
return;
}
// Set ESP-NOW role
esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

// Once ESP-NOW is successfully initialized, register for Send callback
esp_now_register_send_cb(OnDataSent);

// Register broadcast address as a peer
esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}


void loop() {
// Read the soil moisture sensor value
soilMoisture();

// Set the pump status based on the soil moisture value
int pumpStatus;
if (soilMoisturePercent < 30) {
digitalWrite(pumpRelayPin, HIGH);
pumpStatus = 1; // Turn on the pump
}
else {
digitalWrite(pumpRelayPin, LOW);
pumpStatus = 0; // Turn off the pump
}
Serial.println(pumpStatus);
//else {
//pumpStatus = 2; // Keep the pump at its current state
//}

// Set values to send
myData.id = BOARD_ID;
myData.soilMoisturePercent = soilMoisturePercent;
myData.pumpStatus = pumpStatus;

// Send message via ESP-NOW
esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
delay(2000);

}
// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
Serial.print("\r\nLast Packet Send Status: ");
if (sendStatus == 0){
Serial.println("Delivery success");
}
else{
Serial.println("Delivery fail");
}
}
