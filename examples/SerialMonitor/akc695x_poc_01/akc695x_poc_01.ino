#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio; 

void setup() {

  Serial.begin(9600);
  while(!Serial);

  
  
  radio.setup(RESET_PIN);
  radio.setAM(0); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
