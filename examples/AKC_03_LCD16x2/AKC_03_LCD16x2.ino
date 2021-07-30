/*
  It is a FM and AM (LW, MW and SW) receiver based on AKC6955 controlled by Arduino 3.3V/8MHz (Arduino Pro Mini or standalone ATmega328).
  This sketch uses a regular LCD16x2 converted to 3.3V. 

  See user_manual.txt before operating the receiver. 

  Wire up on Arduino UNO, Pro mini and AKC6955
  | Device name               | Device Pin / Description      |  Arduino Pin  |
  | ----------------          | ----------------------------- | ------------  |
  |    LCD 16x2 or 20x4       |                               |               |
  |                           | D4                            |     D7        |
  |                           | D5                            |     D6        |
  |                           | D6                            |     D5        |
  |                           | D7                            |     D4        |
  |                           | RS                            |     D12       |
  |                           | E/ENA                         |     D13       |
  |                           | RW & VSS & K (16)             |    GND        |
  |                           | A (15) & VDD                  |    +Vcc       |
  |                           | VO (see 20K tripot connection)|   ---------   |
  |     AKC6955               |                               |               |
  |                           | RESET (pin 5)                 |      9        |
  |                           | SDIO (pin  6)                 |     A4        |
  |                           | SCLK (pin  7)                 |     A5        |
  |    Encoder                |                               |               |
  |                           | A                             |       2       |
  |                           | B                             |       3       |
  |                           | PUSH BUTTON (encoder)         |     A0/14     |

  Prototype documentation: https://github.com/pu2clr/AKC695X
  PU2CLR Si47XX API documentation: https://pu2clr.github.io/AKC695X/extras/docs/html/index.html

  By PU2CLR, Ricardo, Jul  2021.
*/

#include <AKC695X.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "Rotary.h"


#define RESET_PIN 9

// Enconder PINs
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 4
#define LCD_D6 5
#define LCD_D5 6
#define LCD_D4 7
#define LCD_RS 12
#define LCD_E 13

// Buttons controllers
#define ENCODER_PUSH_BUTTON 14 // Pin A0/14

#define MIN_ELAPSED_TIME 300
#define MIN_ELAPSED_RSSI_TIME 500
#define ELAPSED_COMMAND 2000 // time to turn off the last command controlled by encoder. Time to goes back to the FVO control
#define ELAPSED_CLICK 1500   // time to check the double click commands
#define DEFAULT_VOLUME 36    // change it for your favorite sound volume

#define FM 0
#define LSB 1
#define USB 2
#define AM 3
#define LW 4

#define SSB 1
#define EEPROM_SIZE        512
#define STORE_TIME 10000 // Time of inactivity to make the current receiver status writable (10s / 10000 milliseconds).

// EEPROM - Stroring control variables
const uint8_t app_id = 55; // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;
long storeTime = millis();

bool itIsTimeToSave = false;

uint8_t countClick = 0;

uint8_t seekDirection = 1;

bool cmdBand = false;
bool cmdVolume = false;
bool cmdBandwidth = false;
bool cmdStep = false;
bool cmdMenu = false;


long elapsedRSSI = millis();
long elapsedButton = millis();
long elapsedCommand = millis();
long elapsedClick = millis();
volatile int encoderCount = 0;
uint16_t currentFrequency;
uint16_t previousFrequency = 0;


const char * menu[] = {"Volume", "Step", "BW", "Seek Up", "Seek Down"};
int8_t menuIdx = 0;
const int lastMenu = 4;
int8_t currentMenuCmd = -1;

uint8_t volume = 0;

typedef struct
{
  uint8_t mode;               // 0 = AM; 1 = FM
  uint8_t band;               // AKC695X band (see AKC695X manual)
  uint16_t minimum_frequency; // Minimum frequency to the band
  uint16_t maximum_frequency; // Maximum frequency to the band
  uint16_t currentFreq;       // default frequency or current frequency 
  uint8_t step;               // step used
} akc_band;

akc_band band[] = {
    {1, 1, 760, 1080, 1039, 1},
    {0, 3, 400, 1710, 810, 10},
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
    {0, 0, 150, 285, 200, 3},       // LW   
    {1,  7, 1400, 1480, 1450, 1}};  // VHF / FM

const int lastBand = (sizeof band / sizeof(akc_band)) - 1;
int bandIdx = 0; // FM

typedef struct
{
  uint8_t idx;      
  const char *desc; 
} Bandwidth;

Bandwidth bandwidthFM[] = {
    {2, " 50"},   // 0
    {3, "100"},   // 1 - default BW
    {0, "150"},   // 2 
    {1, "200"}};  // 3 

int8_t bwIdxFM = 1;
const int8_t maxFmBw = 3;    


// Devices class declarations
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
AKC695X rx;

void setup()
{
  // Encoder pins
  pinMode(ENCODER_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  lcd.begin(16, 2);

  // Splash - Remove or Change the splash message
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-AKC6955");
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  delay(2000);
  lcd.clear();
  // End splash

  EEPROM.begin();

  // If you want to reset the eeprom, keep the VOLUME_UP button pressed during statup
  if (digitalRead(ENCODER_PUSH_BUTTON) == LOW)
  {
    EEPROM.update(eeprom_address, 0);
    lcd.setCursor(0,0);
    lcd.print("EEPROM RESETED");
    delay(2000);
    lcd.clear();
  }

  // controlling encoder via interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);

  // rx.setup(RESET_PIN,CRYSTAL_12MHZ);
  rx.setup(RESET_PIN, CRYSTAL_32KHZ);
  delay(500);

  // rx.setFM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency,band[bandIdx].currentFreq, band[bandIdx].step);
  rx.setAudio(); // Sets the audio output behaviour (default configuration).

  


  // Checking the EEPROM content
  if (EEPROM.read(eeprom_address) == app_id)
  {
    readAllReceiverInformation();
  } else 
    rx.setVolume(DEFAULT_VOLUME);
  
  useBand();
  showStatus();
}



/*
   writes the conrrent receiver information into the eeprom.
   The EEPROM.update avoid write the same data in the same memory position. It will save unnecessary recording.
*/
void saveAllReceiverInformation()
{
  int addr_offset;

 
  EEPROM.begin();

  EEPROM.update(eeprom_address, app_id);                 // stores the app id;
  EEPROM.update(eeprom_address + 1, rx.getVolume()); // stores the current Volume
  EEPROM.update(eeprom_address + 2, bandIdx);            // Stores the current band

  addr_offset = 3;
  band[bandIdx].currentFreq = currentFrequency;

  for (int i = 0; i <= lastBand; i++)
  {
    EEPROM.update(addr_offset++, (band[i].currentFreq >> 8));   // stores the current Frequency HIGH byte for the band
    EEPROM.update(addr_offset++, (band[i].currentFreq & 0xFF)); // stores the current Frequency LOW byte for the band
    EEPROM.update(addr_offset++, band[i].step);                 // Stores current step of the band
  }

  EEPROM.end();

}

/**
 * reads the last receiver status from eeprom. 
 */
void readAllReceiverInformation()
{
  uint8_t volume;
  int addr_offset;

  volume = EEPROM.read(eeprom_address + 1); // Gets the stored volume;
  bandIdx = EEPROM.read(eeprom_address + 2);

  addr_offset = 3;
  for (int i = 0; i <= lastBand; i++)
  {
    band[i].currentFreq = EEPROM.read(addr_offset++) << 8;
    band[i].currentFreq |= EEPROM.read(addr_offset++);
    band[i].step = EEPROM.read(addr_offset++);
  }

  currentFrequency = band[bandIdx].currentFreq;
  rx.setVolume(volume);

}

/*
 * To store any change into the EEPROM, it is needed at least STORE_TIME  milliseconds of inactivity.
 */
void resetEepromDelay()
{
  elapsedCommand = storeTime = millis();
  itIsTimeToSave = true;
}

/**
    Set all command flags to false
    When all flags are disabled (false), the encoder controls the frequency
*/
void disableCommands()
{
  cmdBand = false;
  cmdVolume = false;
  cmdBandwidth = false;
  cmdStep = false;
  cmdMenu = false;
  countClick = 0;

  showCommandStatus((char *) "VFO ");
}



/**
 * Reads encoder via interrupt
 * Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
 * if you do not add ICACHE_RAM_ATTR declaration, the system will reboot during attachInterrupt call. 
 * With ICACHE_RAM_ATTR macro you put the function on the RAM.
 */
void  rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
}


/**
 * Converts a number to a char string and places leading zeros. 
 * It is useful to mitigate memory space used by sprintf or generic similar function 
 * 
 * value  - value to be converted
 * strValue - the value will be receive the value converted
 * len -  final string size (in bytes) 
 * dot - the decimal or tousand separator position
 * separator -  symbol "." or "," 
 */
void convertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator)
{
  char d;
  for (int i = (len - 1); i >= 0; i--)
  {
    d = value % 10;
    value = value / 10;
    strValue[i] = d + 48;
  }
  strValue[len] = '\0';
  if (dot > 0)
  {
    for (int i = len; i >= dot; i--)
    {
      strValue[i + 1] = strValue[i];
    }
    strValue[dot] = separator;
  }

  if (strValue[0] == '0')
  {
    strValue[0] = ' ';
    if (strValue[1] == '0')
      strValue[1] = ' ';
  }
}

/**
 * Shows frequency information on Display
 */
void showFrequency()
{
  char *unit;
  char freqDisplay[12];

  currentFrequency = rx.getFrequency();

  if (band[bandIdx].mode == AKC_FM) { // FM
    convertToChar(currentFrequency, freqDisplay, 5, 4, ',');
    unit = (char *)"MHz";    
  } else {
    convertToChar(currentFrequency, freqDisplay, 5, ((currentFrequency < 1000)? 0:2), '.');
    unit = (char *)"kHz";
  }

  strcat(freqDisplay, unit);
  lcd.setCursor(3, 1);
  lcd.print(freqDisplay);  
}


/**
 * Shows some basic information on display
 */
void showStatus()
{
  lcd.clear();
  showFrequency();
  lcd.setCursor(0, 0);
  lcd.print((band[bandIdx].mode == AKC_FM)? "FM":"AM"); 
  showRSSI();
}

/**
 *  Shows the current Bandwidth status
 */
void showBandwidth()
{
  char bwAux[12];
  sprintf(bwAux, "BW: %s", bandwidthFM[bwIdxFM].desc);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(bwAux);  
}

/**
 *   Shows the current RSSI and SNR status
 */
void showRSSI()
{
  int rssi = rx.getRSSI();
  int8_t rssiAux;

  if (rssi < 2)
    rssiAux = 4;
  else if (rssi < 4)
    rssiAux = 5;
  else if (rssi < 12)
    rssiAux = 6;
  else if (rssi < 25)
    rssiAux = 7;
  else if (rssi < 50)
    rssiAux = 8;
  else
    rssiAux = 9;

  lcd.setCursor(13, 1);
  lcd.print('S');
  lcd.print(rssiAux);
  lcd.print( (rssi>= 60)? '+':' ');

}


/**
 *   Shows the current step
 */
void showStep()
{
  char stepAux[12];
  sprintf(stepAux, "STEP: %2u", band[bandIdx].step);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(stepAux);  
}


/*
 *  Shows the volume level on LCD
 */
void showVolume()
{
  char volAux[12];
  sprintf(volAux, "VOLUME: %2u", rx.getVolume());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(volAux);
}


/**
 *   Sets Band up (1) or down (!1)
 */
void setBand(int8_t up_down)
{
  // save the current frequency for the band
  band[bandIdx].currentFreq = currentFrequency;

  if (up_down == 1)
    bandIdx = (bandIdx < lastBand) ? (bandIdx + 1) : 0;
  else
    bandIdx = (bandIdx > 0) ? (bandIdx - 1) : lastBand;
  useBand();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}

/**
 * Switch the radio to current band
 */
void useBand()
{

  if (band[bandIdx].mode == AKC_FM)
  {
    rx.setFM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].currentFreq, band[bandIdx].step);
    rx.setFmSeekStep(0); // 25KHz.
  }
  else
  {
    rx.setAM(band[bandIdx].band, band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].currentFreq, band[bandIdx].step);
  }
  delay(500);
  currentFrequency = band[bandIdx].currentFreq;
  rx.setFrequency(currentFrequency);
  showStatus();
  showCommandStatus((char *) "Band");
}


/**
 *  Switches the Bandwidth
 */
void doBandwidth(int8_t v)
{
  if (band[bandIdx].mode == AKC_FM) {  
    bwIdxFM = (v == 1) ? bwIdxFM + 1 : bwIdxFM - 1;
    if (bwIdxFM > maxFmBw)
      bwIdxFM = 0;
    else if (bwIdxFM < 0)
      bwIdxFM = maxFmBw;
      
   rx.setFmBandwidth(bandwidthFM[bwIdxFM].idx);
   showBandwidth();
  }
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}

/**
 * Show cmd on display. It means you are setting up something.  
 */
void showCommandStatus(char * currentCmd)
{
  lcd.setCursor(5, 0);
  lcd.print(currentCmd);
}

/**
 * Show menu options
 */
void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setCursor(0, 1);
  lcd.print(menu[menuIdx]);
  showCommandStatus( (char *) "Menu");
}



/**
 * Switches the current step
 */
void doStep(int8_t v)
{
    uint8_t step; 
    if (band[bandIdx].mode == AKC_AM) {
      step = (v == 1)? 3: 5;
      rx.setStep(step);
      band[bandIdx].step = step; 
    }
    showStep();
    delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
    elapsedCommand = millis();
}


/**
 * Sets the audio volume
 */
void doVolume( int8_t v ) {
  if ( v == 1)
    rx.setVolumeUp();
  else
    rx.setVolumeDown();

  showVolume();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}


/**
 *  Find a station. The direction is based on the last encoder move clockwise or counterclockwise
 */
void doSeek()
{
  lcd.clear();
  rx.seekStation(seekDirection, showFrequency);
  currentFrequency = rx.getFrequency();
  showStatus();
}


/**
 *  Menu options selection
 */
void doMenu( int8_t v) {
  menuIdx = (v == 1) ? menuIdx + 1 : menuIdx - 1;
  if (menuIdx > lastMenu)
    menuIdx = 0;
  else if (menuIdx < 0)
    menuIdx = lastMenu;

  showMenu();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
  elapsedCommand = millis();
}



/**
 * Return true if the current status is Menu command
 */
bool isMenuMode() {
  return (cmdMenu | cmdStep | cmdBandwidth  | cmdVolume );
}


/**
 * Starts the MENU action process
 */
void doCurrentMenuCmd() {
  disableCommands();
  switch (currentMenuCmd) {
    case 0:                 // VOLUME
      cmdVolume = true;
      showVolume();
      break;
    case 1:                 // STEP
      cmdStep = true;
      showStep();
      break;
    case 2:                 // BW
      cmdBandwidth = true;
      showBandwidth();
      break;
    case 3:
      seekDirection = AKC_SEEK_UP; // SEEK UP
      rx.frequencyUp();
      doSeek();
      break;  
    case 4:
      seekDirection = AKC_SEEK_DOWN; // SEEK DOWN
      rx.frequencyDown();
      doSeek();
      break;    
    default:
      showStatus();
      break;
  }
  currentMenuCmd = -1;
  elapsedCommand = millis();
}



/**
 * Main loop
 */
void loop()
{
  // Check if the encoder has moved.
  if (encoderCount != 0)
  {
    if (cmdMenu)
      doMenu(encoderCount);
    else if (cmdStep)
      doStep(encoderCount);
    else if (cmdBandwidth)
      doBandwidth(encoderCount);
    else if (cmdVolume)
      doVolume(encoderCount);
    else if (cmdBand)
      setBand(encoderCount);
    else
    {
      if (encoderCount == 1)
      {
        rx.frequencyUp();
      }
      else
      {
        rx.frequencyDown();
      }
      // Show the current frequency only if it has changed
      currentFrequency = rx.getFrequency();
      showFrequency();
    }
    encoderCount = 0;
    resetEepromDelay();
  }
  else
  {
    if (digitalRead(ENCODER_PUSH_BUTTON) == LOW)
    {
      countClick++;
      if (cmdMenu)
      {
        currentMenuCmd = menuIdx;
        doCurrentMenuCmd();
      }
      else if (countClick == 1)
      { // If just one click, you can select the band by rotating the encoder
        if (isMenuMode())
        {
          disableCommands();
          showStatus();
          showCommandStatus((char *)"VFO ");
        }
        else
        {
          cmdBand = !cmdBand;
          showCommandStatus((char *)"Band");
        }
      }
      else
      { // GO to MENU if more than one click in less than 1/2 seconds.
        cmdMenu = !cmdMenu;
        if (cmdMenu)
          showMenu();
      }
      delay(MIN_ELAPSED_TIME);
      elapsedCommand = millis();
    }
  }

  // Show RSSI status only if this condition has changed
  if ((millis() - elapsedRSSI) > MIN_ELAPSED_RSSI_TIME * 9)
  {
    showRSSI(); 
    elapsedRSSI = millis();
  }

  // Disable commands control
  if ((millis() - elapsedCommand) > ELAPSED_COMMAND)
  {
    if (isMenuMode()) 
      showStatus();
    disableCommands();
    elapsedCommand = millis();
  }

  if ((millis() - elapsedClick) > ELAPSED_CLICK)
  {
    countClick = 0;
    elapsedClick = millis();
  }

  // Show the current frequency only if it has changed
  if (itIsTimeToSave)
  {
    if ((millis() - storeTime) > STORE_TIME)
    {
      saveAllReceiverInformation();
      storeTime = millis();
      itIsTimeToSave = false;
    }
  }
  delay(2);
}
