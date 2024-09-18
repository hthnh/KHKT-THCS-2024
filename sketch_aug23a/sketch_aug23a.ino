int a0 = A0;
void setup(){
  Serial.begin(9600);

}


void loop(){
  int value = analogRead(a0);
  Serial.println(value);  
  delay(500);
}