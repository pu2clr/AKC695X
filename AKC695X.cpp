#include <AKC695X.h>

/**
 * SI47XX Arduino Library implementation 
 * 
 * This Library is under construction......
 * 
 * This is an Arduino library for the AKC695X, BROADCAST RECEIVER, IC family.  
 * It works with I2C protocol and can provide an easier interface for controlling the AKC695X devices.<br>
 * 
 * This library was built based on [AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio](http://maximradio.altervista.org/akc6955/AKC6955-datasheet-english.pdf) document from "AKC technology". 
 * It also intend to  be used on **all members of the AKC695X family** respecting, of course, the features available for each IC version. 
 * 
 * This library can be freely distributed using the MIT Free Software model. [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/AKC695X/#mit-license).  
 * Contact: pu2clr@gmail.com
 */

/** @defgroup GA03 Basic Methods 
 * @section   GA03 Basic 
 */

/**
 * @ingroup GA03
 * @brief Resets the system. 
 */
void AKC695X::reset() {
    pinMode(this->resetPin, OUTPUT);
    delay(10);
    digitalWrite(this->resetPin, LOW);
    delay(10);
    digitalWrite(this->resetPin, HIGH);
    delay(10);
}

/**
 * @ingroup GA03
 * @brief Sets the I2C bus device address 
 * @details You do not need use this function if your i2c device address is 0x10 (default value)  
 * 
 * @param deviceAddress 
 */
void AKC695X::setI2CBusAddress(int deviceAddress)
{
    this->deviceAddress = deviceAddress;
};

/**
 * @ingroup GA03
 * @brief   Receiver startup 
 * @details 
 * @param resetPin  if >= 0,  then you control the RESET. if -1, you are using ths MCU RST pin. 
 */
void AKC695X::setup(int resetPin)
{
    this->resetPin = resetPin;
    if (resetPin >= 0) reset();
    Wire.begin();
    }

    /**
 * @ingroup GA03
 * @brief Power the device on 
 * 
 * @details  Starts the AKC695X with some parameters.
 * 
 * @param fm_en     1 = FM mode;  0 = AM mode
 * @param tune      If 1 Trigger tune process. The STC bit is set high when the tune operation completes
 * @param mute      If 1 mute L/R audio 
 * @param seek      If 1 Trigger tune process. The STC bit is set high when the tune operation completes
 * @param seekup    Seek direction control bit. 0 = Seek down;  1 = Seek up
 */
    void AKC695X::powerOn(uint8_t fm_en, uint8_t tune, uint8_t mute, uint8_t seek, uint8_t seekup)
    {
        union {
            akc595x_reg0 r;
            uint8_t raw;
        } p;

        p.r.power_on = 1;
        p.r.rsv = 0;
        p.r.fm_en = fm_en;
        p.r.mute = mute;
        p.r.seek = seek;
        p.r.seekup = seekup;
        p.r.tune = tune;

        setRegister(REG00, p.raw);
        this->currentMode = fm_en; // Save the current mode (FM or AM)
}

/**
 * @ingroup GA03
 * @brief Sets a given register with a given value 
 * @details It is a basic function to deal with the AKC695X devices 
 * @param reg  register number to be written (only for RW type registers) 
 * @param parameter  value to be written in the register 
 */
void AKC695X::setRegister(uint8_t reg, uint8_t parameter)
{
    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.write(parameter);
    Wire.endTransmission();
    delayMicroseconds(3000);
}

/**
 * @ingroup GA03
 * @brief Gets a given register content 
 * @details It is a basic function to get a value from a given AKC695X device register
 * @param reg  register number to be read (0 ~ 26) 
 * @return the register content
 */
uint8_t AKC695X::getRegister(uint8_t reg)
{
    uint8_t result;    
    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(this->deviceAddress, 1, false);
    result =  Wire.read();
    Wire.endTransmission(true);
    delay(3);
    return result;
}

/** @defgroup GA04 Receiver Operation  
 * @section   Receiver Operation 
 */

/**
 * @ingroup GA04
 * @brief Sets the STC bit to high when the tune operation completes
 * @details Tells the device that the tune process is over.
 */
void AKC695X::commitTune()
{
    // I know! it can be simpler and faster than this.

    union  {
        akc595x_reg0 r;
        uint8_t raw;
    } reg0;

    reg0.raw = 0; 
    reg0.r.fm_en = this->currentMode;   // Sets to the current mode
    reg0.r.power_on = 1;                
    reg0.r.tune = 1;

    setRegister(REG00, reg0.raw);
    reg0.r.tune = 0;
    setRegister(REG00, reg0.raw);
};

/**
 * @ingroup GA04
 * @brief Sets the AKC695X to FM mode
 * @details Sets the device to FM mode.
 * 
 * | FM band | N#  |Description  |
 * | --------| --- |------------ |
 * | 000     |  0  | FM1,87 ~ 108, station search space specified intervals |
 * | 001     |  1  | FM2,76 ~ 108, station search space specified intervals |
 * | 010     |  2  | FM3,70 ~ 93, with a space station search interval set |
 * | 011     |  3  | FM4,76 ~ 90, Tuning predetermined space intervals |
 * | 100     |  4  | FM5,64 ~ 88, with a space station search interval set | 
 * | 101     |  5  | TV1,56.25 ~ 91.75, station search space specified intervals |
 * | 110     |  6  | TV2, 174.75 ~ 222.25, found |
 * | 111     |  7  | sets predetermined space intervals, custom FM, station search space specified intervals |
 * 
 * @param akc695x_fm_band       FM band (see manual FM band table above)
 * @param minimum_freq          Minimal frequency of the band 
 * @param maximum_freq          Band maximum frequency
 * @param default_frequency     default frequency
 * @param default_step          increment and decrement step 
 */
void AKC695X::setFM(uint8_t akc695x_fm_band, uint16_t minimum_freq, uint16_t maximum_freq, uint16_t default_frequency, uint8_t default_step)
{
    uint16_t channel;
    uint8_t high_bit, low_bit;

    union {
        akc595x_reg1 b;
        uint8_t raw;
    } reg1;

    this->currentMode = 1;
    this->currentBand = akc695x_fm_band;
    this->currentBandMinimumFrequency = minimum_freq;
    this->currentBandMaximumFrequency = maximum_freq;
    this->currentFrequency = default_frequency;
    this->currentStep = default_step;

    reg1.raw = 0;
    reg1.b.fmband = akc695x_fm_band;

    setRegister(REG00, 0b00010011); // Sets to FM (Power On)
    setRegister(REG01, reg1.raw);   /// Sets the FM band

    channel = (default_frequency - 300) * 4;
    high_bit = channel / 256 | 0b01100000;
    low_bit = channel & 0b0000011111111;

    setRegister(REG03, low_bit);
    setRegister(REG02, high_bit);

    commitTune(); 

}

/**
 * @ingroup GA04
 * @brief Sets the AKC695X to AM mode and selects the band
 * @details This method configures the AM band you want to use. 
 * @details You must respect the frequency limits defined by the AKC595X device documentation.
 * 
 * | AM band      | N#  |Description  |
 * | ------------ | --- |------------ |
 * | 00000        |  0  |LW, 0.15 ~ 0.285, 3K station search |
 * | 00001        |  1  |MW1, 0.52 ~ 1.71, 5K station search |
 * | 00010        |  2  |MW2, 0.522 ~ 1.62, 9K station search |
 * | 00011        |  3  |MW3, 0.52 ~ 1.71, 10K station search |
 * | 00100        |  4  |SW1, 4.7 ~ 10, 5K station search |
 * | 00101        |  5  |SW2, 3.2 ~ 4.1, 5K station search |
 * | 00110        |  6  |SW3, 4.7 ~ 5.6, 5K station search |
 * | 00111        |  7  |SW4, 5.7 ~ 6.4, 5K station search |
 * | 01000        |  8  |SW5, 6.8 ~ 7.6, 5K station search |
 * | 01001        |  9  |SW6, 9.2 ~ 10, 5K station search |
 * | 01010        | 10  |SW7, 11.4 ~ 12.2, 5K station search |
 * | 01011        | 11  |SW8, 13.5 ~ 14.3 |
 * | 01100        | 12  |SW9, 15 ~ 15.9 |
 * | 01101        | 13  |SW10, 17.4 ~ 17.9 |
 * | 01110        | 14  |SW11, 18.9 ~ 19.7, 5K station search |
 * | 01111        | 15  |SW12, 21.4 ~ 21.9, 5K station search |
 * | 10000        | 16  |SW13, 11.4 ~ 17.9, 5K station search |
 * | 10010        | 17  |MW4, 0.52 to 1.73, 5K station search |
 * | Other        | 18+ |custom band, station search interval = 3K |
 * 
 * @param akc695x_am_band       AM band (see manual AM band table above)
 * @param minimum_freq          Minimal frequency of the band 
 * @param maximum_freq          Band maximum frequency
 * @param default_frequency     default frequency
 * @param default_step          increment and decrement step 
 */
void AKC695X::setAM(uint8_t akc695x_am_band, uint16_t minimum_freq, uint16_t maximum_freq, uint16_t default_frequency, uint8_t default_step)
{
    uint16_t channel;
    uint8_t  high_bit, low_bit;

    union {
        akc595x_reg1 b;
        uint8_t raw;
    } reg1;

    this->currentMode = 0;
    this->currentBand = akc695x_am_band;
    this->currentBandMinimumFrequency = minimum_freq;
    this->currentBandMaximumFrequency = maximum_freq;
    this->currentFrequency = default_frequency;
    this->currentStep = default_step;

    reg1.raw = 0;
    reg1.b.amband = akc695x_am_band;

    setRegister(REG00, 0b10000000); // Sets to AM (Power On)   
    setRegister(REG01, reg1.raw);   // Selects the AM band
    
    channel = default_frequency / this->currentStep;
    high_bit = channel / 256 | 0b01100000;
    low_bit  = channel & 0b0000011111111;

    setRegister(REG03, low_bit);
    setRegister(REG02, high_bit);

    commitTune();
}

/**
 * @ingroup GA04
 * @brief Sets the step that will be used to increment and decrement the current frequency
 * @details The AKC695X has two possible steps (3KHz and 5KHz). The step value is important to calculate the frequenvy you want to set. 
 * 
 * @see setFrequency
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 * 
 * @param step  The valid values are 3 and 5. Other values will be ignored.
 */
void AKC695X::setStep(uint8_t step)
{
    this->currentStep = step;
}

/**
 * @ingroup GA04
 * @brief Sets FM step for seeking. 
 * 
 * @details Sets FM seek step. 
 * 
 * | spece | N#  | step    |
 * | ----- | --- | ------- | 
 * |  00   |  0  | 25 KHz  |
 * |  01   |  1  | 50 KHz  | 
 * |  10   |  2  | 100 KHz | 
 * |  11   |  3  | 200 KHz | 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 * 
 * @param uint8_t space the value betwenn 0 and 3 (see table above). 
 */
void AKC695X::setFmSeekStep(uint8_t space)
{
    union {
        akc595x_reg11 r;
        uint8_t raw;
    } reg11; 
    reg11.r.space = (space > 3)? 3 : space;
    setRegister(REG11, reg11.raw);
}

/**
 * @ingroup GA04
 * @brief Seeks a FM station 
 * @details Seek a FM Station
 * @param up_down  if 0, seek down; if 1, seek up.
 */
void AKC695X::seekFmStation(uint8_t up_down)
{
    union {
        akc595x_reg0 r;
        uint8_t raw;
    } reg0;

    reg0.raw = 0;

    reg0.r.fm_en = 1;       // FM
    reg0.r.mute = 0;        // Normal operation
    reg0.r.power_on = 1;    // Power on
    reg0.r.tune = 1;        // Trigger tune process
    reg0.r.seek = 1;        // ? Trigger seeking process ?
    reg0.r.seekup = up_down;

    setRegister(REG00, reg0.raw);

}

/**
 * @ingroup GA04
 * @brief Sets the the device to a given frequency
 * 
 * @details This methods check the current mode (AM or FM), calculates the right channel to be setted.
 * @details Sets to reg2 structure the 5 most significan bist of the channel.
 * @details Sets to reg3 the 8 less significant bits of the channel.
 * 
 * @see akc595x_reg2, akc595x_reg3
 * 
 * @param frequency frequency you want to set to 
 */
void AKC695X::setFrequency(uint16_t frequency)
{

    uint16_t channel;
    union {
        akc595x_reg2 r;
        uint8_t raw;
    } reg2;

    uint8_t reg3;

    reg2.raw = getRegister(REG02);                      // Gets the current value of the REG02

    if (this->currentMode == 0) 
    {
        // AM mode
        channel = frequency / this->currentStep;
        reg2.r.channel = channel >> 8 | 0b100000;      // Changes just the 5 higher bits of the channel.
        reg3 = channel & 0b0000011111111;              // Sets the 8 lower bits of the channel 

        setRegister(REG03, reg3);
        setRegister(REG02, reg2.raw);
    }
    else
    {
        // FM mode
        channel = (frequency - 300) * 4;
        reg2.r.channel = channel >> 8 | 0b100000;
        reg3 = channel & 0b0000011111111;
        setRegister(REG03, reg3);
        setRegister(REG02, reg2.raw);
    }

    commitTune();

    this->currentFrequency = frequency;
}

/**
 * @ingroup GA04
 * @brief  Returns the current frequency value
 * @details Gets the current frequency.
 * @return uint16_t  Current frequency value.
 */
uint16_t AKC695X::getFrequency()
{
    return this->currentFrequency;
}

/**
 * @ingroup GA04
 * @brief Adds the current step to the current frequency and sets the new frequency
 * @details Goes to the next frequency channel  
 */
void AKC695X::frequencyUp()
{
    this->currentFrequency += this->currentStep;
    setFrequency(this->currentFrequency);
}

/**
 * @ingroup GA04
 * @brief Subtracts the current step from the current frequency and assign the new frequency
 * @details Goes to the previous frequency channel  
 */
void AKC695X::frequencyDown()
{
    this->currentFrequency -= this->currentStep;
    setFrequency(this->currentFrequency);
}

/**
 * @ingroup GA04
 * @brief Configures the audio output
 * 
 * @details This method sets the AKC695X device audio behaviour
 * 
 * @param phase_inv if 0, audio output inphase; if 1, audio output inverted
 * @param line      if 0, audio input mode; if 1, radio mode.
 * @param volume    if 25 a 63, audio volume; if <= 24 mute 
 */
void AKC695X::setAudio(uint8_t phase_inv, uint8_t line, uint8_t volume)
{
    union {
        akc595x_reg6 a;
        uint8_t raw;
    } reg6;

    this->volume = reg6.a.volume = (volume > 63) ? 63 : volume;
    reg6.a.line = line;
    reg6.a.phase_inv = phase_inv;
    setRegister(REG06,reg6.raw);

}

/**
 * @ingroup GA04
 * @brief Configures the audio output with default values
 * @details this method sets the audio phase_inv = 0; line = 1 and volume = 40;
 * @details Also, this set the audio controlled by MCU (Arduino)
 * 
 * @see setVolumeControl
 * 
 */
void AKC695X::setAudio()
{
    setVolumeControl(1); // Audio controlled by MCU
    setAudio(0, 0, 40);
}

/**
 * @ingroup GA04
 * @brief Sets the output audio volume
 * @details Values less than 24 mute the audio output. 
 * @details Values between 25 and 63 set the output audio volume.
 * @param volume 
 */

void AKC695X::setVolume(uint8_t volume)
{
    union {
        akc595x_reg6 v;
        uint8_t raw;
    } reg6;

    reg6.raw = getRegister(REG06);          // gets the current register value;

    if ( volume > 63 ) volume = 63;
    this->volume = reg6.v.volume = volume;  // changes just the volume attribute 
    setRegister(REG06, reg6.raw);           // writes the new reg9 value
}

/**
 * @ingroup GA04
 * @brief Increments the audio volume 
 * @details The maximum volume is 63
 */
void AKC695X::setVolumeUp() {
    this->volume = (this->volume > 63) ? 63 : (this->volume + 1);
    setVolume(this->volume);
}

/**
 * @ingroup GA04
 * @brief Decrements the audio volume 
 * @details The minimum volume is 25
 */
void AKC695X::setVolumeDown()
{
    this->volume = (this->volume < 25 ) ? 25 : (this->volume - 1);
    setVolume(this->volume);
}

/**
 * @brief  Sets the kind of audio volume control will be used.
 * @details This method configures the kind of audio volume control will be used. 
 * @details You can control the audio volume by potentiometer or by MCU (Arduino). 
 * @details If you choose volume conttolled by Arduino (type 1), you can set the volume from 25 to 63 levels.
 *  
 * @see setVolume, akc595x_reg9
 * 
 * @param type  0 = controlled by poteciometer; 1 controlled by the MCU 
 */
    void AKC695X::setVolumeControl(uint8_t type)
{
    union {
        akc595x_reg9 vc;
        uint8_t raw;
    } reg9;

    reg9.raw = getRegister(REG09); // gets the current register value;
    reg9.vc.pd_adc_vol = type;     // changes just the attribute pd_adc_vol
    setRegister(REG09, reg9.raw);  // writes the new reg9 value 
}

/**
 * @ingroup GA04
 * @brief Gets the current RSSI
 * @details Gets the current RSSI
 * @return int  RSSI value
 */
int AKC695X::getRSSI()
{
    union {
        akc595x_reg27 r;
        uint8_t raw;
    } reg27;

    reg27.raw = getRegister(REG27);

    return reg27.r.rssi;
}

/**
 * @ingroup GA04
 * @brief Gets the supply voltage 
 * @details Gets the current supply voltage 
 * @return float the supply voltage
 */
float AKC695X::getSupplyVoltage()
{
    union {
        akc595x_reg25 r;
        uint8_t raw;
    } reg25;

    reg25.raw = getRegister(REG25);

    return (1.8 + 0.05 * reg25.r.vbat);
}
