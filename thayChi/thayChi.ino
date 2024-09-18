int sensorPin = A0;
int relayPin = 8;
int dryPoint = 700;
void setup() {

pinMode(relayPin, OUTPUT);

}

void loop() {
  int value = analogRead(sensorPin);
  if(value > dryPoint){
    digitalWrite(relayPin, HIGH);
    delay(5000);
    digitalWrite(relayPin, LOW);
  }
  delay(60000);
}
