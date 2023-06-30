#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include "esp_wpa2.h" 
#include <Firebase_ESP_Client.h>
// #include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

              // Provide the token generation process info.
              #include "addons/TokenHelper.h"
              // Provide the RTDB payload printing info and other helper functions.
              #include "addons/RTDBHelper.h"

#define EAP_ANONYMOUS_IDENTITY "anonymous@iitr.ac.in" //anonymous@example.com, or you can use also nickname@example.com
#define EAP_IDENTITY "19915031" //nickname@example.com, at some organizations should work nickname only without realm, but it is not recommended
#define EAP_PASSWORD "Aranyacc20@" //password for eduroam account
//SSID NAME
const char* ssid = "IITR_WIFI";

// Insert Firebase project API Key
#define API_KEY "AIzaSyBcICVWYSSwPne1a5gjTdScTTftjpET7E0"
#define ss 15
#define rst 13
#define dio0 12



String receivedData;
//float sensor1, sensor2, sensor3, sensor4;

//OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "kramitsingh197@gmail.com"
              #define USER_PASSWORD "Amit@20041"

              // Insert RTDB URLefine the RTDB URL
              #define DATABASE_URL "https://farmlink-in-default-rtdb.asia-southeast1.firebasedatabase.app/"

              // Define Firebase objects
              FirebaseData fbdo;
              FirebaseAuth auth;
              FirebaseConfig config;

              // Variable to save USER UID
              String uid;

              // Database main path (to be updated in setup with the user UID)
              String databasePath;
              // Database child nodes
              String temp1Path = "/temp1";
              String hum1Path = "/hum1";
              String temp2Path = "/temp2";
              String hum2Path = "/hum2";
              String temp3Path = "/temp3";
              String hum3Path = "/hum3";
              String temp4Path = "/temp4";
              String hum4Path = "/hum4";
              String temp5Path = "/temp5";
              String hum5Path = "/hum5";

              String timePath = "/timestamp";

              // Parent Node (to be updated in every loop)
              String parentPath;

              int timestamp;
              FirebaseJson json;

              const char* ntpServer = "pool.ntp.org";

float temp1,temp2,temp3,temp4,hum1,hum2,hum3,hum4;


                  // Timer delay 
                  unsigned long sendDataPrevMillis = 0;
                  unsigned long timerDelay = 5000;
                  // Function that gets current epoch time
            unsigned long getTime() {
              time_t now;
              struct tm timeinfo;
              if (!getLocalTime(&timeinfo)) {
                //Serial.println("Failed to obtain time");
                return(0);
              }
              time(&now);
              return now;
            }

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }

  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect from WiFi to set new WiFi connection
  WiFi.mode(WIFI_STA); //init wifi mode

  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); //WITHOUT CERTIFICATE - WORKING WITH EXCEPTION ON RADIUS SERVER

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP()); //print LAN IP address
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
      
  configTime(0, 0, ntpServer);
  // Assign the api key (required)
  config.api_key = API_KEY;
  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;
  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";



  display.clearDisplay();                   // clear the display's memory
  display.display();                        // display the cleared memory
  display.setTextSize(1);                   // set text size
  display.setTextColor(WHITE); 

   display.clearDisplay();
   display.setTextSize(2);
   display.setCursor(3,5);
   display.println("   Soil & ");
   display.println("  Weather ");
   display.println("  Monitor ");
   display.display();
   delay(2000);
  
   display.clearDisplay();
   display.setTextSize(2);
   display.setCursor(0,1);
   display.println("Developed  by  ");
   display.println("   IIT  ");
   display.println(" ROORKEE");
   display.display();
   delay(1000);

  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(15,0);
  display.print("LoRA Initialised");
  display.display();
  delay(1000);

display.clearDisplay();
display.setTextSize(1);
display.setCursor(0,1);
display.println("Loop Chaalu");
display.display();
delay(1000);


}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      receivedData = LoRa.readString();
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(15,0);
    display.print("Data Available");
    display.display();
    }

    String sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7, sensor8, sensor9, sensor10;
    int comma1 = receivedData.indexOf(":");
    int comma2 = receivedData.indexOf(":", comma1 + 1);
    int comma3 = receivedData.indexOf(":", comma2 + 1);
    int comma4 = receivedData.indexOf(":", comma3 + 1);
    int comma5 = receivedData.indexOf(":", comma4 + 1);
    int comma6 = receivedData.indexOf(":", comma5 + 1);
    int comma7 = receivedData.indexOf(":", comma6 + 1);
    int comma8 = receivedData.indexOf(":", comma7 + 1);
    int comma9 = receivedData.indexOf(":", comma8 + 1);
    sensor1 = receivedData.substring(0, comma1);//soil moisture 
    sensor2 = receivedData.substring(comma1 + 1, comma2);//pump status
    sensor3 = receivedData.substring(comma2 + 1, comma3);//rain status
    sensor4 = receivedData.substring(comma3 + 1, comma4);//ambient temp
    sensor5 = receivedData.substring(comma4 + 1, comma5);//ambient humid
    sensor6 = receivedData.substring(comma5 + 1, comma6);//soil temp
    sensor7 = receivedData.substring(comma6 + 1, comma7);//node 1 moisture
    sensor8 = receivedData.substring(comma7 + 1, comma8);//node 1 pump
    sensor9 = receivedData.substring(comma8 + 1, comma9);//mode 2 moisture 
    sensor10 = receivedData.substring(comma9 + 1);//node2  pump

    Serial.println(sensor1);
    Serial.println(sensor2);
    Serial.println(sensor3);
    Serial.println(sensor4);
    Serial.println(sensor5);
    Serial.println(sensor6);
    Serial.println(sensor7);
    Serial.println(sensor8);
    Serial.println(sensor9);
    Serial.println(sensor10);
    

    
    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

 
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Soil Moisture: " + sensor1);
    display.setCursor(0, 20);
    display.println("Pump Status: " + sensor2);
    display.display();
    delay(1000);
    display.clearDisplay();

    display.setCursor(0, 10);
    display.println("Rain Status: " + sensor3);
    display.setCursor(0, 20);
    display.println("Amb. Temp.: " + sensor4);
    display.display();
    delay(1000);
    display.clearDisplay();

    display.setCursor(0, 10);
    display.println("Amb. Humm.: " + sensor5);
    display.setCursor(0, 20);
    display.println("Soil Temp." + sensor6);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Moisture (N_1): " + sensor7);
    display.setCursor(0, 20);
    display.println("PUMP (N_1): " + sensor8);
    display.display();
    delay(1000);
    display.clearDisplay();

    display.setCursor(0, 10);
    display.println("Moisture (N_2): " + sensor9);
    display.setCursor(0, 20);
    display.println("PUMP (N_2): " + sensor10);
    display.display();
    delay(1000);
    display.clearDisplay();

    parentPath= databasePath + "/" + String(timestamp);
    
    json.set(temp1Path.c_str(), sensor4);//ambient temp
    json.set(temp2Path.c_str(), sensor5);//ambient humidity
    json.set(temp3Path.c_str(), sensor1);//soil moisture main
    json.set(temp4Path.c_str(), sensor6);//soil temperature
    json.set(temp5Path.c_str(), sensor2);//pump main on/off
    json.set(hum1Path.c_str(), sensor3);//rain status
    json.set(hum2Path.c_str(), sensor7);//board1 moisture
    json.set(hum3Path.c_str(), sensor8);//board1 pump
    json.set(hum4Path.c_str(), sensor9);//board2 moisture
    json.set(hum5Path.c_str(), sensor10);//board2 pump
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  

  }
}
