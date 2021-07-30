/**
 * It is FM and AM (MW and SW) AKC6955 based receiver controlled by a Arduino Pro Mini. You can also use 
 * other Arduino boards. However, see documentation before try that: https://github.com/pu2clr/AKC695X 
 * 
 * This receiver use a OLED 92" (128 x 64), 6 push buttons and an Encoder. 
 *  
 * This sketch uses the following libraries:
 *  PU2CLR AKC Arduino Library;
 *  Adafruit_GFX; and 
 *  Adafruit_SSD1306
 *  
 * This software can be freely distributed using the MIT Free Software model.
 * 
 * AKC6955 and Arduino Pro Mini wire up
 * 
 * | AKC6955 pin | Arduino pin | Description | 
 * | ----------- | ----------- | ----------- |
 * |    5        |   12        | You can use the Arduino RST pin. In this case set RESET_PIN to -1 |
 * |    6 (SCL)  |   A5 SCL    | I2C bus clock | 
 * |    7 (SDA)  |   A4 SDA    | I2C bus data  | 
 * 
 *    
 * @see schematic on https://github.com/pu2clr/AKC695X#schematic
 * 
 * @author Ricardo Lima Caratti (pu2clr@gmail.com)
 * @copyright Copyright (c) 2020
 */

#include <AKC695X.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Rotary.h"

#define RESET_PIN 12 // You can use the Arduino RST pin. In this case, set it to -1

#define OLED_I2C_ADDRESS 0x3C   // Check the I2C bus of your OLED device
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Buttons controllers
#define BT_BAND_MODE_SWITCH_UP 4     // Switch to: FM -> MW(AM) -> SW1(AM) -> SW2(AM) etc
#define BT_BAND_MODE_SWITCH_DOWN 5   //
#define BT_VOL_UP    6               // Volume Up
#define BT_VOL_DOWN  7               // Volume Down
#define BT_SEEK_UP   8               // Seek Station Up
#define BT_SEEK_DOWN 9               // Seek Station Down

#define MIN_ELAPSED_TIME 100

long elapsedButton = millis();


typedef struct
{
  uint8_t mode;               // 0 = AM; 1 = FM
  uint8_t band;               // AKC695X band (see AKC695X manual)
  uint16_t minimum_frequency; // Minimum frequency to the band
  uint16_t maximum_frequency; // Maximum frequency to the band
  uint16_t default_frequency; // default frequency
  uint8_t step;               // step used
} akc_band;

akc_band band[] = {
    {1, 1, 760, 1080, 1039, 1},
    {0, 3, 520, 1710, 810, 5},
    {0, 6, 4700, 5600, 4885, 5}, 
    {0, 7, 5700, 6400, 6100, 5},    
    {0, 8, 6800, 7600, 7205, 5},
    {0, 9, 9200, 10500, 9600, 5},
    {0, 10, 11400, 12200, 11940, 5},
    {0, 11, 13500, 14300, 13600, 5},
    {0, 12, 15000, 15900, 15300, 5},
    {0, 13, 17400, 17900, 17600, 5},  
    {0, 15, 21400, 21900, 21525, 5},
    {0, 18, 27000, 28000, 27500, 3},
    {1,  7, 1400, 1480, 1450, 1}};

const int lastBand = (sizeof band / sizeof(akc_band)) - 1;
int bandIdx = 0; // FM

// The array sizes below can be optimized. 
char oldFreq[20];
char oldMode[20];
char oldUnit[20];
char oldStep[20];
char oldRssi[20];
char oldVolume[20];
char oldVbat[20];
char oldStereo[20];

Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);
// Encoder control variables
volatile int encoderCount = 0;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AKC695X radio;

uint16_t currentFM = 103;
uint16_t currentAM = 810;

uint16_t currentFrequency;

void setup()
{

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  pinMode(BT_BAND_MODE_SWITCH_UP, INPUT_PULLUP);
  pinMode(BT_BAND_MODE_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(BT_VOL_UP, INPUT_PULLUP);
  pinMode(BT_VOL_DOWN, INPUT_PULLUP);
  pinMode(BT_SEEK_UP, INPUT_PULLUP);
  pinMode(BT_SEEK_DOWN, INPUT_PULLUP);  

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);


  oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  splash();

  // You can select the RESET pin and Crystal type you are using in your circuit. 
  // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select CRYSTAL_32KHz or CRYSTAL_12MHZ
  radio.setup(RESET_PIN, CRYSTAL_32KHz);

  radio.setFM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency,band[bandIdx].default_frequency, band[bandIdx].step);
  radio.setAudio(); // Sets the audio output behaviour (default configuration).
  
  showStatus();
}

void rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
  {
    if (encoderStatus == DIR_CW)
    {
      encoderCount = 1;
    }
    else
    {
      encoderCount = -1;
    }
  }
}

/**
 * Clear a given array char string
 */
inline void clearBuffer(char *p)
{
  p[0] = '\0';
}

/**
 * Chear all string buffer information
 * These strings are used to avoid blinking on display.
 * See printValue function.
 */
void resetBuffer()
{
  clearBuffer(oldFreq);
  clearBuffer(oldMode);
  clearBuffer(oldUnit);
  clearBuffer(oldStep);
  clearBuffer(oldRssi);
  clearBuffer(oldVolume);
  clearBuffer(oldVbat);
  clearBuffer(oldStereo);
}

/**
 * Splash - Change it for your introduction text.
 */
void splash()
{
  oled.setTextSize(1); // Draw 2X-scale text
  oled.setCursor(40, 0);
  oled.print("AKC695X");
  oled.setCursor(20, 10);
  oled.print("Arduino Library");
  oled.display();
  delay(500);
  oled.setCursor(30, 35);
  oled.print("OLED-EXAMPLE");
  oled.setCursor(10, 50);
  oled.print("V1.0.0 - By PU2CLR");
  oled.display();
  delay(2000);
}

/*
    Writes just the changed information on Display
    Prevents blinking on display and also noise.
    Erases the old digits if it has changed and print the new digit values.
*/
void printValue(int col, int line, char *oldValue, char *newValue, int space, int textSize)
{
  int c = col;
  char *pOld;
  char *pNew;

  pOld = oldValue;
  pNew = newValue;

  oled.setTextSize(textSize);

  // prints just changed digits
  while (*pOld && *pNew)
  {
    if (*pOld != *pNew)
    {
      oled.setTextColor(SSD1306_BLACK);
      oled.setCursor(c, line);
      oled.write(*pOld);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(c, line);
      oled.write(*pNew);
    }
    pOld++;
    pNew++;
    c += space;
  }
  // Is there anything else to erase?
  oled.setTextColor(SSD1306_BLACK);
  while (*pOld)
  {
    oled.setCursor(c, line);
    oled.write(*pOld);
    pOld++;
    c += space;
  }
  // Is there anything else to print?
  oled.setTextColor(SSD1306_WHITE);
  while (*pNew)
  {
    oled.setCursor(c, line);
    oled.write(*pNew);
    pNew++;
    c += space;
  }

  // Save the current content to be tested next time
  strcpy(oldValue, newValue);
}

/**
 * Show the current frequency
 * 
 */
void showFrequency()
{
  char freq[15];
  char tmp[15];
  char *unit;
  char *bandMode;
  char *stereo;
  

  currentFrequency = radio.getFrequency();

  sprintf(tmp, "%5u", currentFrequency);

  if (band[bandIdx].mode == 1) // FM
  {
    freq[0] = tmp[0];
    freq[1] = tmp[1];
    freq[2] = tmp[2];
    freq[3] = tmp[3];
    freq[4] = '.';
    freq[5] = tmp[4];    
    freq[6] = '\0';
    unit = (char *) "MHz";
    bandMode =  (char *) "FM";
    stereo = (radio.isFmStereo())? (char *) "Stereo": (char *)"Mono";
  }
  else // AM
  {
    freq[0] = ' ';
    freq[1] = tmp[0];
    freq[2] = tmp[1];
    freq[3] = tmp[2];
    freq[4] = tmp[3];
    freq[5] = tmp[4];
    freq[6] = '\0';
    unit = (char *) "kHz";
    bandMode = (char *) "AM";
    stereo = (char *) "       ";
  }

  printValue(23, 0, oldFreq, freq, 12, 2);

  printValue(0, 20, oldStereo, stereo, 6, 1);

  printValue(0, 0, oldMode, bandMode, 7, 1);
  printValue(105, 0, oldUnit, unit, 7, 1);
  showRSSI();

  oled.display();
}

// Show current frequency
void showStatus()
{

  oled.clearDisplay();
  resetBuffer();

  oled.drawLine(0, 17, 130, 17, SSD1306_WHITE);
  oled.drawLine(0, 52, 130, 52, SSD1306_WHITE);

  showFrequency();

  showVolume();
  showVbat();

  oled.display();
}

/* *******************************
   Shows RSSI status
*/
void showRSSI()
{

  char sR[20];

  sprintf(sR,"RSSI:%3.3idBuV", radio.getRSSI());
  printValue(0, 40, oldRssi, sR, 6, 1);
  oled.display();

}

/*
   Shows the volume level on LCD
*/
void showVolume()
{
  char sVolume[20];
  sprintf(sVolume, "Vol: %2.2u", radio.getVolume());
  printValue(80, 56, oldVolume, sVolume, 6, 1);
  oled.display();
}

/*
 * Shows the battery voltage 
 */
void showVbat()
{
  char sV[20];
  float v = radio.getSupplyVoltage();
  dtostrf(v, 2, 1, sV); // This may not work on some architectures (for example: Arduino DUE). 
  strcat(sV,"V");
  printValue(0, 56, oldVbat, sV, 6, 1);
  oled.display();
 
}

/*********************************************************/

/*
 * Button - Volume control
 */
void volumeButton(byte d)
{
  
  if (d == 1)
    radio.setVolumeUp();
  else
    radio.setVolumeDown();

  showVolume();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}


void seekButton( uint8_t up_down ) {
    // Tells to the radio.seekStation your function that shows the frequency. In this case showFrequency.
    // If you don't want to show the frequency during the seek process, just call radio.seekStation(up_down);
    radio.seekStation(up_down,showFrequency);  
    showFrequency();
}


void bandUp()
{
  // save the current frequency for the band
  band[bandIdx].default_frequency = currentFrequency;

  if (bandIdx < lastBand)
  {
    bandIdx++;
  }
  else
  {
    bandIdx = 0;
  }
  useBand();
}



void bandDown()
{
  // save the current frequency for the band
  band[bandIdx].default_frequency = currentFrequency;
  
  if (bandIdx > 0)
  {
    bandIdx--;
  }
  else
  {
    bandIdx = lastBand;
  }
  useBand();
}


void useBand() {

  if (band[bandIdx].mode ==  1) 
  {
    radio.setFM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
    radio.setFmSeekStep(0); // 25kHz.
  }
  else
  {
    radio.setAM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
  }
  delay(100);
  currentFrequency = band[bandIdx].default_frequency;
  radio.setFrequency(currentFrequency);

  showStatus();
}

void loop()
{
  // Check if the encoder has moved.
  if (encoderCount != 0)
  {
    if (encoderCount == 1)
      radio.frequencyUp();
    else
      radio.frequencyDown();

    showFrequency();
    encoderCount = 0;
  }

  // Check button commands
  if ((millis() - elapsedButton) > MIN_ELAPSED_TIME)
  {
    // check if some button is pressed
    if (digitalRead(BT_BAND_MODE_SWITCH_UP) == LOW)
       bandUp(); 
    else if (digitalRead(BT_BAND_MODE_SWITCH_DOWN) == LOW)
      bandDown();
    else if (digitalRead(BT_VOL_UP) == LOW)
      volumeButton(1);
    else if (digitalRead(BT_VOL_DOWN) == LOW)
      volumeButton(-1);
    else if  (digitalRead(BT_SEEK_UP) == LOW)
      seekButton(1);
    else if  (digitalRead(BT_SEEK_DOWN) == LOW)
      seekButton(0);
  }  

  delay(10);
}
