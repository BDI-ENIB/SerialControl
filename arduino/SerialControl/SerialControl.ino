int led = LED_BUILTIN;
int i = 0;
bool spam = false; 

void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(2000000);    
}

void loop() {
  
	digitalWrite(led,LOW);
  	if(Serial.available() > 0) {
    
	// read the incoming message
    String message = Serial.readStringUntil(';');

	//then ccall function depending on message
    if(message.equals("whois")) {

	  /*it is recommanded to avaoid println because it insert
	  unvanted \n*/
      Serial.print("I'm_Groot;");
      digitalWrite(led,HIGH);
      delay(100); // just for the led, nothing to do with serial
    }

	if(message.equals("activate")) {
      spam = true;
	}

  	}
	
	if(spam) {
		Serial.print("spam;");
	}

}
