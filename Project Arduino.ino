#include <SimpleDHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal.h>
#include <Time.cpp>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "NHC M-Ustaw"
#define WIFI_PASSWORD "Pmakungu@123"

//#define WIFI_SSID "strathmore"
//#define WIFI_PASSWORD "5trathm0re"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDQR9tFsaN7GU9eGrBJQ0ZJI2597egD_7w"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://maji-mkulima-base-default-rtdb.europe-west1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int pinDHT11 = D4;

// Valve 
int valvePin = D0; 
int dir1= D1;
int dir2 = D2;
int valveStatus = 0;

/*time_t t(){
    while( millis() - sendDataPrevMillis >= 1000){      
      sysTime++;
      sendDataPrevMillis += 1000;  
  #ifdef TIME_DRIFT_INFO
      sysUnsyncedTime++; // this can be compared to the synced time to measure long term drift     
  #endif  
    }
    if(nextSyncTime <= sysTime){
    if(getTimePtr != 0){
      time_t t = getTimePtr();
        if( t != 0)
          setTime(t);
        else
          Status = (Status == timeNotSet) ?  timeNotSet : timeNeedsSync;        
      }
    }  
    return sysTime;
  }

*/


SimpleDHT11 dht11(pinDHT11);

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(valvePin, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  Serial.println("Soil Moisture, Amospheric Temperature and Humidity");
  Serial.println("=================================");

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  // start working...
  
    
  // read dht values without samples.
  
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(5000);
    return;
  }
  double bmoist = analogRead(A0); // moisture readings
  double moist = (1024 - bmoist)/1024*100; 
  Serial.print("Temperature: ");
  Serial.print((double)temperature); Serial.print(" *C, Humidity: "); 
  Serial.print((float)humidity); Serial.print(" %, Soil Moisture: "); 
  Serial.print((double)moist); Serial.println(" %");

  digitalWrite(dir1, HIGH);// direction port
  digitalWrite(dir2, LOW);
  
  if(temperature > 27.00||humidity < 50.00||moist > 30){
    valveStatus = 0;
    digitalWrite(valvePin, valveStatus);
    Serial.println("Valve Closed\n");
    // the motor is off when non of the direction ports is on
  }
  else{
    valveStatus = 1;
    digitalWrite(valvePin, valveStatus);
    Serial.println("Valve Open\n");
    // the motor is engaged when one direction port is on
  }

    
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "Data/DataID", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    if (Firebase.RTDB.setFloat(&fbdo, "Data/DataID/Temperature °C", temperature)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "Data/DataID/Humidity Percentage", humidity)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "Data/DataID/Soil Moisture Level Percentage", moist)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if(valveStatus==0){
      if (Firebase.RTDB.setString(&fbdo, "Data/DataID/Valve Status", "Closed")){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
    else {
      if (Firebase.RTDB.setString(&fbdo, "Data/DataID/Valve Status", "Open")){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }
  delay(2000);
}
