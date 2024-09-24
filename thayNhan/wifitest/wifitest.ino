#include <WiFiS3.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "ArduinoJson.h"

#define ssid "hth"
#define pass "haithinh"

const char* host = "http://192.168.1.2:8080";
IPAddress server(192,168,1,2);
IPAddress local_IP(192,168,1,80);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
int status = WL_IDLE_STATUS;
int id;
String local;
int sensor_A = A5;
int warnPoint = 300;


ArduinoLEDMatrix matrix;
WiFiClient client;


void setup(){
  delay(5000);
  Serial.begin(9600);
  while(!Serial){
    delay(5000);
  }
  matrix.begin();
  printLedMatrix("    Start");
  


  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    printLedMatrix("error");
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    printLedMatrix("error");

  }
  WiFi.config(local_IP, gateway, subnet);
  
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    printLedMatrix("   waiting for wifi");
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
    printLedMatrix(" error");
    Serial.println("error get reg");
    while(true);
  }

  if(resp_code() == 404){
    printLedMatrix(" error");
    Serial.println(" New Register");
    while(true);
  }


  // Wait for the server to respond
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

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




// void get_data(){
//   printLedMatrix("   Register Sensor");
//   if(client.connect(server,8080))
//   Serial.println("connected get value");
//   client.println("GET / HTTP/1.1");
//   client.println("");
//   client.println("Connection: close");

// }

void post_warning(){
  if(client.connect(server,8080))
  Serial.println("connected post value");
  // Prepare HTTP POST request
  String url = "/Warning";
  String data = "{\"No\":\"\",\"Local\":\""+ local +"\",\"From\":"+ id +", \"Time\":\"\", \"Date\":\"\"}";

  DynamicJsonDocument jsonDoc(1024);  // Adjust size as needed
  jsonDoc["No"] = "";  // Replace with your data
  jsonDoc["Local"] = local;       // Replace with your data
  jsonDoc["From"] = id;       // Replace with your data
  jsonDoc["Time"] = "";       // Replace with your data
  jsonDoc["Date"] = "";       // Replace with your data
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // // Build the HTTP POST request
  // client.print(String("POST ") + url + " HTTP/1.1\r\n" +
  //              "Host: " + host + "\r\n" +
  //              "Content-Type: application/x-www-form-urlencoded\r\n" +
  //              "Content-Length: " + data.length() + "\r\n" +
  //              "Connection: close\r\n\r\n" +
  //              data + "\r\n");
  

  client.println("POST /Warning HTTP/1.1");
  client.println("Host: "+ host);
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


void loop() {

  int value = analogRead(sensor_A);
  Serial.println(value);
  while(value > warnPoint){
    post_warning();
    if(resp_code() == 201) break;
    else {
      Serial.println("Post error");
      printLedMatrix("   error");
    }
    
  }

  // delay(10000);

  // while(true){
  //   post_warning();
  //   Serial.println(resp_code());
  //   if(resp_code() == 201) break;
  //   else {
  //     Serial.println("Post error");
  //     printLedMatrix("   error");

  //   }
    
  // }


  
}
