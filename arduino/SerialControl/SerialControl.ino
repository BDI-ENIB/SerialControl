int led = 13;
int i = 0;

void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(9600);     
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    String message = Serial.readStringUntil(';');
    if(message.equals("whois")) {
      Serial.println("I'm Groot;");
    }
  }
}
