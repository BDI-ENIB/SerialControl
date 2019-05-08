int led = LED_BUILTIN;
int i = 0;
bool spam = false; 

void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(9600);    
}

void loop() {
  digitalWrite(led,LOW);
  if (Serial.available() > 0) {
    // read the incoming byte:
    String message = Serial.readStringUntil(';');
    if(message.equals("whois")) {
      spam = true;
      Serial.println("I'm_Jesus;");
      digitalWrite(led,HIGH);
      delay(100);
    }
    
  }
}
