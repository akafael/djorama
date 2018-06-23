
#define PORT_MIC A0
#define PORT_LED 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PORT_LED,OUTPUT);
  pinMode(PORT_MIC,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int micInput = analogRead(PORT_MIC);
  if(micInput > 650)
  {
    digitalWrite(PORT_LED,HIGH);
  }
  else
  {
    digitalWrite(PORT_LED,LOW);
  }
  Serial.println(micInput);
}
