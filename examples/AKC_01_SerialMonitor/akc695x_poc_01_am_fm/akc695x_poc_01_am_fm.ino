#include <AKC695X.h>


#define RESET_PIN 12   // set it to -1 if you want to use the RST pin of your MCU.

AKC695X radio;

uint16_t currentFM = 1039;
uint16_t currentAM = 810;

uint16_t currentFrequency;

void setup() {

  Serial.begin(9600);
  // while (!Serial);

  showHelp();
  // You can select the RESET pin and Crystal type you are using in your circuit.
  // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select CRYSTAL_32KHz or CRYSTAL_12MHZ
  // radio.setup(RESET_PIN, CRYSTAL_12MHZ);
  radio.setup(RESET_PIN, CRYSTAL_12MHZ);
  // radio.setup(RESET_PIN, CRYSTAL_32KHz);
  delay(500);
  currentFrequency = currentFM = 1039;
  radio.setFM(0, 870, 1080, currentFrequency, 1);
  
  radio.setAudio(); // Sets the audio output behaviour (default configuration). 

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

  char  freq[15];

  currentFrequency = radio.getFrequency();

  sprintf(freq,"%5.5u", currentFrequency); // (radio.getCurrentMode() == CURRENT_MODE_FM)?  currentFrequency / 10.0 : currentFrequency / 1000.0;

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
        currentFM = radio.getFrequency();
        radio.setAM(3,520, 1710, currentAM, 10);
        break;
      case 'f':
      case 'F':
        currentAM = radio.getFrequency();
        radio.setFM(0, 870, 1080,currentFM, 1);
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
        radio.seekStation(AKC_SEEK_UP);
        break;      
      case 's':
        radio.seekStation(AKC_SEEK_DOWN);      
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
