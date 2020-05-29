#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio;

uint16_t currentFM = 103;
uint16_t currentAM = 810;

uint16_t currentFrequency;

void setup() {

  Serial.begin(9600);
  while (!Serial);


  radio.setup(RESET_PIN);

  currentFrequency = currentFM = 1039;
  radio.setFM(0, 870, 1080, currentFrequency, 1);
  
  radio.setAudio(); // Sets the audio output behaviour (default configuration). 

  showHelp();
  showStatus();
}


void showHelp()
{
  Serial.println("Type F to FM; A to MW; 1 to SW");
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Show current frequency
void showStatus()
{

  float freq;

  currentFrequency = radio.getFrequency();

  freq =  (radio.getCurrentMode() == CURRENT_MODE_FM)?  currentFrequency / 10.0 : currentFrequency / 1000.0;

  Serial.print("\nYou are tuned on ");
  Serial.print(freq);
  Serial.print("MHz - RSSI: ");
  Serial.print(radio.getRSSI());
  Serial.print(" - Battery: ");
  Serial.print(radio.getSupplyVoltage());
  Serial.print("V - Volume: ");
  Serial.print(radio.getVolume());

}

void loop() {

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
        currentFM = currentFrequency;
        radio.setAM(3,550, 1710, currentAM, 5);
        break;
      case 'f':
      case 'F':
        currentAM = currentFrequency;
        radio.setFM(0, 87.0, 108.0,currentFM, 1);
        break;
      case '1':
        radio.setAM(10,11400, 12200, 11940, 5);
        break;      
        break;
      case 'U':
      case 'u':
        radio.frequencyUp();
        break;
      case 'D':
      case 'd':
        radio.frequencyDown();
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

    showStatus();
  }


}
