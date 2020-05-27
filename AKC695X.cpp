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

/**
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
 * @brief Receiver startup 
 * @todo
 * @param resetPin 
 */
void AKC695X::setup(int resetPin)
{
    digitalWrite(resetPin, HIGH);
    this->resetPin = resetPin;
}

/**
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
 * @brief Gets a given register content 
 * @details It is a basic function to get a value from a given AKC695X device register
 * @param reg  register number to be read (0 ~ 26) 
 * @return the register content
 */
uint8_t AKC695X::getRegister(uint8_t reg)
{
    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.endTransmission();
    delayMicroseconds(3000);
    Wire.requestFrom(this->deviceAddress, 1);

    return Wire.read();
}

/**
 * @brief Sets the AKC695X to FM mode
 * 
 */
void AKC695X::setFM(uint8_t akc695x_fm_band, float minimum_freq, float maximum_freq,float default_frequency)
{
    uint16_t channel;
    uint8_t high_bit, low_bit;

    this->currentMode = 1;
    this->amCurrentBand = akc695x_fm_band;
    this->amCurrentBandMinimumFrequency = minimum_freq;
    this->amCurrentBandMaximumFrequency = maximum_freq;
    this->currentFrequency = default_frequency;

        /* 
    union {
        akc595x_reg1 b;
        uint8_t raw;
    } reg1;

    this->currentMode = 1;

    this->amCurrentBand = akc695x_fm_band;
    this->amCurrentBandMinimumFrequency = minimum_freq;
    this->amCurrentBandMaximumFrequency = maximum_freq;

    reg1.b.fmband = akc695x_fm_band;
    setRegister(REG01, reg1.raw);
    powerOn(1, 1, 0, 0, 0);
    */

    setRegister(REG00, 0b00010011); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
    setRegister(REG01, 0b11000000); ///AM-band
    setRegister(REG04, 0x00);
    setRegister(REG05, 0xff);

    channel = (default_frequency - 30) * 40;
    high_bit = channel / 256 | 0b01100000;
    low_bit = channel & 0b0000011111111;

    setRegister(REG03, low_bit);
    setRegister(REG02, high_bit);
    setRegister(REG00, 0b11100000);
    setRegister(REG00, 0b11000000);
}

/**
 * @brief Sets the AKC695X to AM mode and selects the band
 * @details This method configures the AM band you want to use. 
 * @details You must respect the frequency limits defined by the AKC595X device documentation.
 * 
 * @param akc695x_band  AM band number (check the AM table band on AKC695X documentation).
 * @param minimum_freq  AM band minimum frequency used for the band (check the AM table band on AKC695X documentation). 
 * @param maximum_freq  AM band maximum frequency used for the band (check the AM table band on AKC695X documentation).
 */
void AKC695X::setAM(uint8_t akc695x_am_band, float minimum_freq, float maximum_freq, float default_frequency)
{
    uint16_t channel;
    uint8_t  high_bit, low_bit;

    /*
    union {
        akc595x_reg1 b;
        uint8_t raw;
    } reg1;

    this->currentMode = 0;
    this->amCurrentBand = akc695x_am_band;
    this->amCurrentBandMinimumFrequency = minimum_freq;
    this->amCurrentBandMaximumFrequency = maximum_freq;

    reg1.b.amband = akc695x_am_band;
    setRegister(REG01, reg1.raw);
    powerOn(1,0,0,0,0);
    */
    setRegister(REG00, 0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
    setRegister(REG01, 0b00010011); ///AM-band
    setRegister(REG04, 0x00);
    setRegister(REG05, 0xff);
    
    channel = default_frequency / 3;
    high_bit = channel / 256 | 0b01100000;
    low_bit  = channel & 0b0000011111111;
    setRegister(REG03, low_bit);
    setRegister(REG02, high_bit);
    setRegister(REG00, 0b10100000);
    setRegister(REG00, 0b10000000);

}

/**
 * @brief Sets the step that will be used to increment and decrement the current frequency
 * @details The AKC695X has two possible steps (3KHz and 5KHz). The step value is important to calculate the frequenvy you want to set. 
 * 
 * @see setFrequency
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 * 
 * @param step  The valid values are 3 and 5. Other values will be ignored.
 */
void AKC695X::setStep(int step)
{
    if (step == 3 || step == 5)
        this->currentStep = step;
}

/**
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
void AKC695X::setFrequency(float frequency)
{
    uint16_t channel;
    uint8_t high_bit, low_bit;

    union {
        akc595x_reg2 r;
        uint8_t raw;
    } reg2;

    if (this->currentMode == 0) // AM mode
    {
        // TODO
        channel = (frequency / this->currentStep);
    }
    else
    { // FM mode
        setRegister(REG00, 0b00010011); ///power_on,AM, tune0,seek0,seek_down,non_mute,00
        setRegister(REG01, 0b11000000); ///AM-band
        setRegister(REG04, 0x00);
        setRegister(REG05, 0xff);

        channel = (frequency - 30) * 40;
        high_bit = channel / 256 | 0b01100000;
        low_bit = channel & 0b0000011111111;

        setRegister(REG03, low_bit);
        setRegister(REG02, high_bit);
        setRegister(REG00, 0b11100000);
        setRegister(REG00, 0b11000000);
    }
    // Gets the current Reg2 value and change just the channel value
    reg2.raw = getRegister(REG02);
    reg2.r.channel = channel >> 8; // Sets to reg2 structure the 5 most significan bist of the channel
    setRegister(REG02, reg2.raw);  
    setRegister(REG03, channel & 0b0000011111111); // Sets to reg3 the 8 less significant bits of the channel.

    this->currentFrequency = frequency;
}

/**
 * @brief  Returns the current frequency value
 * 
 * @return uint16_t  Current frequency value.
 */
float AKC695X::getFrequency()
{
    return this->currentFrequency;
}

/**
 * @brief Adds the current step to the current frequency and sets the new frequency
 * 
 */
void AKC695X::frequencyUp()
{
    this->currentFrequency += this->currentStep;
    setFrequency(this->currentFrequency);
}

/**
 * @brief Subtracts the current step from the current frequency and assign the new frequency
 * 
 */
void AKC695X::frequencyDown()
{
    this->currentFrequency -= this->currentStep;
    setFrequency(this->currentFrequency);
}


/**
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
 * @brief Increments the audio volume 
 * @details The maximum volume is 63
 */
void AKC695X::setVolumeUp() {
    this->volume = (this->volume > 63) ? 63 : (this->volume + 1);
    setVolume(this->volume);
}

/**
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
