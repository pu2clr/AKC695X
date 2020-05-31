#include <AKC695X.h>


#define RESET_PIN 12

AKC695X radio;

uint16_t currentFrequency;

void setup() {

  Serial.begin(9600);
  while (!Serial);

  // You can select the RESET pin and Crystal type you are using in your circuit.
  // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select CRYSTAL_32KHZ or CRYSTAL_12MHZ
  // radio.setup(RESET_PIN, CRYSTAL_12MHZ);
  radio.setup(RESET_PIN);

  currentFrequency = 11140;
  radio.setAM(10,11400, 12200, 11940, 5);
  radio.setAudio(); // Sets the audio output behaviour (default configuration). 

  showHelp();
  showStatus();
}


void showHelp()
{
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Show current frequency
void showStatus()
{

  currentFrequency = radio.getFrequency();

  Serial.print("\nYou are tuned on ");
  Serial.print(currentFrequency / 1000.0);
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
      case 'U':
      case 'u':
        radio.frequencyUp();
        break;
      case 'D':
      case 'd':
        radio.frequencyDown();
        break;
      case '1':
          currentFrequency = 9600;
          radio.setAM(10,11400, 12200, currentFrequency, 5);   
          break;   
      case '2':
          currentFrequency = 11140;
          radio.setAM(10,11400, 12200, currentFrequency, 5);
          break;
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
