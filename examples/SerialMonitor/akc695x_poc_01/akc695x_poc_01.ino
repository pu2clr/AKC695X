#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio; 

void setup() {

  Serial.begin(9600);
  while(!Serial);

  
  
  radio.setup(RESET_PIN);
  radio.setAM(3, 520, 1710); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
