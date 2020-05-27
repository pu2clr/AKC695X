#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio; 


float currentFM = 103.9;
float currentAM = 810.0;
float currentFrequency;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  
  radio.setup(RESET_PIN);
 
  currentFrequency = 95.5;
  radio.setFM(0, 87.0, 108.0, currentFrequency);
  radio.setVolumeControl(1); // The volume will be controlled by the MCU.
  radio.setVolume(55);   
 
  showHelp();  
}


void showHelp()
{
  Serial.println("Type F to FM; A to MW; 1 to All Band (100KHz to 30MHz)");
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type B to change Bandwith filter");
  Serial.println("Type 4 to 8 (4 to step 1; 5 to step 5Khz; 6 to 10KHz; 7 to 100Khz; 8 to 1000KHz)");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Show current frequency
void showStatus()
{
  currentFrequency = radio.getFrequency();

  Serial.print("\nYou are tuned on ");
  Serial.print(currentFrequency);
  Serial.print("MHz ");
}

void loop() {
  /*
  if (Serial.available() > 0)
  {
    char key = Serial.read();
    switch (key)
    {
    case '+':
      radio.setVolumeUp();
      break;
    case '-':
      radio.setVolumeDown();
      break;
    case 'a':
    case 'A':
      break;
    case 'f':
    case 'F':
       radio.setFM(0, 87.0, 108.0, 95.5);
      break;
    case '1':
      break;
    case 'U':
    case 'u':
      radio.frequencyUp();
      showStatus();
      break;
    case 'D':
    case 'd':
      radio.frequencyDown();
      showStatus();
      break;
    case 'S':
    case 's':
      break;
    case '0':
      showStatus();
      break;
    case '?':
      showHelp();
      break;
    default:
      break;
    }
  }
  */
 
}
