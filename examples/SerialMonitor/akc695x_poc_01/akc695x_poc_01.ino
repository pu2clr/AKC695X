#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio; 

void setup() {
  // put your setup code here, to run once:

  radio.setup(RESET_PIN);
  radio.setAM(0); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
