#include <Wire.h>
#include "AHT20.h"  // Include the AHT20 library

// Define sensor pins
#define MQ7_PIN A0        // MQ-7 analog output connected to A0
#define VOC_PIN A1        // MQ-7 analog output connected to A0

// Initialize AHT20 sensor
AHT20 aht20;

// Variables to store sensor readings
int mq7Value = 0;
int VOCValue = 0;
float temperature = 0.0;
float humidity = 0.0;

// Timestamp variables
unsigned long previousMillis = 0;
const long interval = 1000; // Interval at which to send data (milliseconds)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize I2C communication for AHT20
  Wire.begin();
  
  // Initialize AHT20 sensor
  if (aht20.begin()) {
    Serial.println("AHT20 sensor initialized successfully.");
  } else {
    Serial.println("ERROR: Failed to initialize AHT20 sensor!");
    while (1); // Stop execution
  }
  if (aht20.available() == true){
    Serial.println("ERROR: Failed to initialize AHT20 sensor!");
    while (1); // Stop execution
  }

  // Allow sensors to stabilize
  delay(2000); // 2 seconds
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read MQ-7 sensor value
    mq7Value = analogRead(MQ7_PIN);
    VOCValue = analogRead(VOC_PIN);
    
    // Read temperature and humidity from AHT20
    temperature = aht20.getTemperature();
    humidity = aht20.getHumidity();
    
    // Get current timestamp in milliseconds since Arduino started
    unsigned long timestamp = millis();
    
    // Send data in CSV format: Timestamp,MQ7_Value,Temperature,Humidity
    Serial.print(timestamp);
    Serial.print(",");
    Serial.print(mq7Value);
    Serial.print(",");
    Serial.print(VOCValue);
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.println(humidity);
    
    // Optional: Print to Serial Monitor for debugging
    /*
    Serial.print("Timestamp: ");
    Serial.print(timestamp);
    Serial.print(" ms, MQ7: ");
    Serial.print(mq7Value);
    Serial.print(", Temp: ");
    Serial.print(temperature);
    Serial.print(" C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    */
  }
}
