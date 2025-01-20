#include <WiFiS3.h>
#include <Wire.h>
#include "AHT20.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "ArduinoJson.h"

// Define wifi
#define ssid "NGUOIVIETGOCCAY"
#define pass "12345678"
// Define pin
#define MQ7_PIN A0
#define VOC_PIN A1
//initialize AHT20
AHT20 aht20;

// client config
const char* host = "http://192.168.69.15:8080";
IPAddress server(192,168,69,15);
IPAddress local_IP(192,168,69,80);
IPAddress gateway(192,168,69,255);
IPAddress subnet(255,255,255,0);
int status = WL_IDLE_STATUS;
String local;
WiFiClient client;
int id;

// define store variable
int mq7Value;
int vocValue;
int temperature;
int humidity;


// led matrix 
ArduinoLEDMatrix matrix;


void setup(){
  delay(5000);
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  while(!Serial){
    delay(500);
  }
  matrix.begin();
  printLedMatrix("    Start  ");
  

  Wire.begin();

  if (aht20.begin()) {
    Serial.println("AHT20 sensor initialized successfully.");
  } else {
    Serial.println("ERROR: Failed to initialize AHT20 sensor!");
    while (1) printLedMatrix("  121  "); // Stop execution
  }



  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    printLedMatrix("error ");
    while (true){
      printLedMatrix("  101  ");
    }
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    printLedMatrix("  error 002  ");

  }
  WiFi.config(local_IP, gateway, subnet);
  
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    printLedMatrix("   waiting for wifi  ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
     
    delay(10000);
  }
  
  printWifiStatus();

  Serial.println("\n Connected");
  printLedMatrix("    Connected");
  
  delay(2000);

  printLedMatrix("   Register Sensor");
  if(client.connect(server,8080)){
    Serial.println("Register start");
    client.println("GET /ArClients/ip/" + local_IP.toString() + " HTTP/1.1");
    client.println("");
    client.println("Connection: close");
  }else{
    printLedMatrix(" error ");
    Serial.println("error get reg");
    while(true) printLedMatrix("  111  ");
    
  }

  if(resp_code() == 404){
    printLedMatrix(" error");
    Serial.println(" New Register");
    while(true) printLedMatrix("  112  ");
  }


  // Wait for the server to respond
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  delay(2000);

  // Read the response body
  String response = "";
  while (client.available()) {
    response += client.readString();
  }

  DynamicJsonDocument doc(1024);  // Adjust the size based on your expected JSON size
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    printLedMatrix(" error ");
    while(true) printLedMatrix("  113  ");
  }

  id = doc["id"];
  local = doc["local"].as<String>();

  client.stop();
  

}


int resp_code(){
  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');
    if (line.startsWith("HTTP/1.1") || line.startsWith("HTTP/1.0")) {
      // Extract the response code from the status line
      int responseCode = line.substring(9, 12).toInt();
      return responseCode;
      break;
    }
  }
  return 404;
}

void printLedMatrix(String text){
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);

  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
  delay(2000);
}


/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}




void post_warning(){
  if(client.connect(server,8080))
  Serial.println("connected post value");
  // Prepare HTTP POST request
  String url = "/Warning";
  String data = "{\"No\":\"\",\"Local\":\""+ local +"\",\"From\":"+ id +", \"Time\":\"\", \"Date\":\"\"}";

  DynamicJsonDocument jsonDoc(1024); 
  jsonDoc["No"] = "";  
  jsonDoc["Local"] = local;       
  jsonDoc["From"] = id;       
  jsonDoc["Time"] = "";       
  jsonDoc["Date"] = "";       
  String jsonString;
  serializeJson(jsonDoc, jsonString);


  client.println("POST /Warning HTTP/1.1");
  client.println(String("Host: ")+ host);
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(jsonString.length()));
  client.println(); 
  client.println(jsonString);  // Send JSON data
  client.println("Connection: close");

}



void post_data(int Co, int Voc, int Temp, int Hum){
  if(client.connect(server,8080))
  Serial.println("connected post value");

  // Prepare data request

  DynamicJsonDocument jsonDoc(1024); 
  jsonDoc["ID"] = id;
  jsonDoc["Co"] = Co;       
  jsonDoc["VOC"] = Voc;       
  jsonDoc["Temp"] = Temp;       
  jsonDoc["Hum"] = Hum;       
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Prepare HTTP POST request

  client.println("POST /ReceiveData HTTP/1.1");
  client.println(String("Host: ")+ host);
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(jsonString.length()));
  client.println(); 
  client.println(jsonString);  // Send JSON data
  client.println("Connection: close");

}



void read_response() {
/* -------------------------------------------------------------------------- */  
  while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    // Read the response
    String response = client.readString();
    Serial.println("Response:");
    Serial.println(response);

    client.stop();
}


void get_sensor_value(){
  mq7Value = analogRead(MQ7_PIN);
  vocValue = analogRead(VOC_PIN);
  temperature = aht20.getTemperature();
  humidity = aht20.getHumidity();
}

void main_func(){
  if( resp_code() == 301){
  fan();
  post_warning();
}else{
  int totalCo = 0;
  int totalVoc = 0;
  int totalTemp = 0;
  int totalHum = 0;
  fan();

  for(int i = 0; i < 5; i++){
    get_sensor_value();
    totalCo += mq7Value;
    totalVoc += vocValue;
    totalTemp += temperature;
    totalHum += humidity;
  fan();

    delay(1000);
  }
  fan();

  post_data(totalCo / 5, totalVoc / 5, totalTemp / 5, totalHum / 5);
  fan();

  Serial.print(totalCo / 5);
  Serial.print(", ");
  Serial.print(totalVoc / 5);
  Serial.print(", ");

  Serial.print(totalTemp / 5);
  Serial.print(", ");

  Serial.println(totalHum / 5);
  }
}
void fan(){
  if(digitalRead(1) == 0){
    digitalWrite(3,HIGH);
    delay(1500);
  }

  if(digitalRead(2) == 0){
    digitalWrite(3,LOW);
    delay(1500);
  }
  
}

void loop(){
  fan();
  main_func();

}

