#include <WiFiS3.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#define ssid "hth1"
#define pass "haithinh"

const char* host = "192.168.10.129";
IPAddress server(192,168,23,149);
IPAddress local_IP(192,168,23,80);
IPAddress gateway(192,168,23,1);
IPAddress subnet(255,255,255,0);
int status = WL_IDLE_STATUS;


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




void get_data(){
  printLedMatrix("   Register Sensor");
  if(client.connect(server,8080))
  Serial.println("connected get value");
  client.println("GET / HTTP/1.1");
  client.println("");
  client.println("Connection: close");

}

void post_value(){
  if(client.connect(server,8080))
  Serial.println("connected post value");
  // Prepare HTTP POST request
  String url = "/";
  String data = "id-sensor=1&values-sensor=200";

  // Build the HTTP POST request
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + data.length() + "\r\n" +
               "Connection: close\r\n\r\n" +
               data + "\r\n");
  

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

  


  // // if the server's disconnected, stop the client:
  // if (!client.connected()) {
  //   Serial.println();
  //   Serial.println("disconnecting from server.");
  //   client.stop();

  //   // do nothing forevermore:
  //   while (true);
  // }
}
