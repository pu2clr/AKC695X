#include <AKC695X.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Rotary.h"

#define RESET_PIN 12 // You can use the Arduino RST pin. In this case, set it to -1

#define OLED_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4     // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Buttons controllers
#define BAND_MODE_SWITCH_UP 4   // Switch to: FM -> MW(AM) -> SW1(AM) -> SW2(AM) etc
#define BAND_MODE_SWITCH_DOWN 5 //
#define VOL_UP 6                // Volume Up
#define VOL_DOWN 7              // Volume Down

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
    {0, 8, 6800, 7600, 7205, 5},
    {0, 9, 9200, 10500, 9600, 5},
    {0, 10, 11400, 12200, 11940, 5}};

const int lastBand = (sizeof band / sizeof(akc_band)) - 1;
int bandIdx = 0; // FM

char oldFreq[20];
char oldMode[10];
char oldUnit[10];
char oldStep[10];
char oldExtraSignalInfo[15];
char oldRssi[20];
char oldVolume[10];

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

  pinMode(BAND_MODE_SWITCH_UP, INPUT_PULLUP);
  pinMode(BAND_MODE_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DOWN, INPUT_PULLUP);

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);


  oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  splash();

  radio.setup(RESET_PIN);
  radio.setFM(band[bandIdx].band, band[bandIdx].maximum_frequency, band[bandIdx].maximum_frequency,band[bandIdx].default_frequency, band[bandIdx].step);
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
  clearBuffer(oldExtraSignalInfo);
  clearBuffer(oldRssi);
  clearBuffer(oldVolume);
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

  byte textSize;

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
    unit = "MHz";
    bandMode = "FM";
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
    unit = "KHz";
    bandMode = "AM";
  }

  printValue(23, 0, oldFreq, freq, 12, 2);

  printValue(0, 0, oldMode, bandMode, 7, 1);
  printValue(105, 0, oldUnit, unit, 7, 1);

  oled.display();
}

// Show current frequency
void showStatus()
{
  char step[10];

  oled.clearDisplay();
  resetBuffer();

  oled.drawLine(0, 17, 130, 17, SSD1306_WHITE);
  oled.drawLine(0, 52, 130, 52, SSD1306_WHITE);

  showFrequency();

  showVolume();
  showRSSI();

  oled.display();
}

/* *******************************
   Shows RSSI status
*/
void showRSSI()
{
}

/*
   Shows the volume level on LCD
*/
void showVolume()
{
  char sVolume[10];
  sprintf(sVolume, "V%2.2u", radio.getVolume());
  printValue(105, 30, oldVolume, sVolume, 6, 1);
  oled.display();
}

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

  radio.setFM(band[bandIdx].band, band[bandIdx].maximum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);

  if (band[bandIdx].mode ==  1) 
  {
    radio.setFM(band[bandIdx].band, band[bandIdx].maximum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
  }
  else
  {
    radio.setAM(band[bandIdx].band, band[bandIdx].maximum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
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
    if (digitalRead(BAND_MODE_SWITCH_UP) == LOW)
       bandUp(); 
    else if (digitalRead(BAND_MODE_SWITCH_DOWN) == LOW)
      bandDown();
    else if (digitalRead(VOL_UP) == LOW)
      volumeButton(1);
    else if (digitalRead(VOL_DOWN) == LOW)
      volumeButton(-1);
  }  

  delay(10);
}
