/**
 * @mainpage AKC695X Arduino Library implementation
 *
 * This is an Arduino library for the AKC695X, BROADCAST RECEIVER, IC family.
 * It works with I2C protocol and can provide an easier interface for controlling the AKC695X devices.<br>
 *
 * This library was built based on [AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio](http://maximradio.altervista.org/akc6955/AKC6955-datasheet-english.pdf) document from "AKC technology".
 * Others sources help the author to build this library.  They are referenced in the documentation for this library on: https://github.com/pu2clr/AKC695X
 * It also intend to  be used on **all members of the AKC695X family** respecting, of course, the features available for each IC version.
 *
 * This library uses the I2C protocols to read and write AKC695X registers. In this context, registers are memory position into the device.
 * The first 13 registers, you can use to change the behavior of the device. They are read and write registers. The registers from 20 to 27, can be used to get the current device status. They are read only registers.
 * There is no information about the register 10 and the registers 14 to 19 in the documentation used to develop this library.
 * By using the registers 0 to 13, you can change the band, set the frequency, set the channel space, set the audio behavior and volume, set a custom band and more.
 *
 * This library can be freely distributed using the MIT Free Software model. [Copyright (c) 2019 Ricardo Lima Caratti](https://pu2clr.github.io/AKC695X/#mit-license).
 * Contact: pu2clr@gmail.com
 */

#ifndef _AKC6955_H // Prevent this file from being compiled more than once
#define _AKC6955_H

#include <Arduino.h>
#include <Wire.h>

#define DEFAUL_I2C_ADDRESS 0x10
#define CURRENT_MODE_FM     1
#define CURRENT_MODE_AM     0
#define CRYSTAL_12MHZ       0
#define CRYSTAL_32KHz       1

#define MAX_SEEK_TIME   3000        // Maximum time have to be a seeking process (in ms).
#define AKC_SEEK_UP 1
#define AKC_SEEK_DOWN 0
#define AKC_FM 1
#define AKC_AM 0

/**
 * @brief AKC695X features
 * @details the table below shows some features fo the AKC695X devices family
 *
 * | Feature                                                                   | AKC6951 | AKC6955 | AKC6959 |
 * | --------------------------------------------------------------------------| ------- | ------- | ------- |
 * | stand by 30 ~ 230MHz Any frequency FM demodulation                        |   X     |    X    |    X    |
 * | stand by 0.15 ~ 30MHz Any frequency AM demodulation                       |   X     |    X    |    X    |
 * | Preset FM band ( 64 ~ 108 MHz )                                           |   X     |    X    |    X    |
 * | stand by TV1 audio( 56.25 ~ 91.75 MHz )                                   |   X     |    X    |    X    |
 * | stand by TV2 audio( 174.75 ~ 222.25 MHz )                                 |   X     |    X    |    X    |
 * | Presets wave band ( 520 ~ 1730 kHz )                                      |   X     |    X    |    X    |
 * | __Preset shortwave band ( 3.2 ~ 21.9 MHz )__                              |         |    X    |    X    |
 * | Preset long-wave band ( 150 ~ 285 kHz )                                   |   X     |    X    |    X    |
 * | Custom support band                                                       |   X     |    X    |    X    |
 * | It supports a wide supply voltage range: 2.0V ~ 4.5V                      |   X     |    X    |    X    |
 * | stand by 32.768kHz & 12MHz Passive mode crystal reference clock and Pin   |   X     |    X    |    X    |
 * | Integrated audio amplifier (maximum power differential 0.5W )             |   X     |    X    |    X    |
 * | Two kinds of control volume: volume potentiometer and the volume register |   X     |    X    |    X    |
 * | Two kinds of de-emphasis modes: 50us / 75us                               |   X     |    X    |    X    |
 * | Support tuning lamp function                                              |   X     |    X    |    X    |
 * | The audio output may be in phase, may be inverted output                  |   X     |    X    |    X    |
 * | Support for stereo line input                                             |   X     |    X    |    X    |
 * | Support low-power standby mode, 3V When power consumption 10uA            |   X     |    X    |    X    |
 * | Internal integrated PLL                                                   |   X     |    X    |    X    |
 * | Intelligent frequency control                                             |   X     |    X    |    X    |
 * | AGC                                                                       |   X     |    X    |    X    |
 * | Precise digital demodulation                                              |   X     |    X    |    X    |
 * | Smart mute function                                                       |   X     |    X    |    X    |
 * | SW Increased tracking filter, greatly improving SW Audibility             |   X     |    X    |    X    |
 * | MW Precise tuning adaptive front end                                      |   X     |    X    |    X    |
 * | FM Subwoofer                                                              |   X     |    X    |    X    |
 * | integrated LDO                                                            |   X     |    X    |    X    |
 * | According to the battery voltage, automatically adjust the volume         |   X     |    X    |    X    |
 * | Pb-free / RoHS complian                                                   |   X     |    X    |    X    |
 */

#define AKC695X_I2C_ADRESS 0x10

// Read and Write (RW) AKC695X registers
#define REG00 0x00
#define REG01 0x01
#define REG02 0x02
#define REG03 0x03
#define REG04 0x04
#define REG05 0x05
#define REG06 0x06
#define REG07 0x07
#define REG08 0x08
#define REG09 0x09
#define REG11 0x0B
#define REG12 0x0C
#define REG13 0x0D
// Read only AKC695X registers
#define REG20 0x14
#define REG21 0x15
#define REG22 0x16
#define REG23 0x17
#define REG24 0x18
#define REG25 0x19
#define REG26 0x1A
#define REG27 0x1B

/**
 * @defgroup GA01 Union, Structure and Defined Data Types
 * @brief   AKC695X Defined Data Types
 * @details Defined Data Types is a way to represent the AKC695X registers information
 * @details Some information appears to be inaccurate due to translation problems from Chinese to English.
 * @details The information shown here was extracted from Datasheet:
 * @details AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation.
 * @details Other information seems incomplete even in the original Chinese Datasheet.
 * @details For example: Reg 10 (0x0A). There is no information about it. The Reg11 and 12 seem wrong
 */

/**
 * @ingroup GA01
 * @brief Reg0 (type 0x00 / RW): configures register 0 (default: 0x4c) Address
 * @see  AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 11
 */

typedef union {
    struct
    {
        uint8_t rsv : 2;      //!< Reserved - Debug use, do not change this value using
        uint8_t mute : 1;     //!< 1 - Mute L / R channel 0 - Normal operation
        uint8_t seekup : 1;   //!< Seek direction control bit. 0 = Seek down;  1 = Seek up
        uint8_t seek : 1;     //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes.
        uint8_t tune : 1;     //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes.
        uint8_t fm_en : 1;    //!< 1 = FM mode;  0 = AM mode
        uint8_t power_on : 1; //!< 1 = Chip on; 0 = Chip off
    } refined;
    uint8_t raw;
} akc595x_reg0;

/**
 * @ingroup GA01
 * @brief Reg1 (type 0x01 / RW): configures register 1 (default: 0x10) Address
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 *
 */
typedef union {
    struct
    {
        uint8_t fmband : 3; //!<
        uint8_t amband : 4; //!<
    } refined;
    uint8_t raw;
} akc595x_reg1;

/**
 * @ingroup GA01
 * @brief Reg2 (type 0x02 / RW): configure register 2 (default: 0x4A) Address
 *
 * @details High channel number 5, the channel number and frequencies related as follows:
 * @details FM mode: Channel Freq = 25kHz * CHAN + 30MHz AM mode, when 5K channel number pattern:.
 * @details Channel Freq = 5kHz * CHAN AM mode, 3K channel number pattern. when: Channel Freq = 3kHz * CHAN.
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 *
 */
typedef union {
    struct
    {
        uint8_t channel : 5;      //!< (0:4) - 5 most significant bits that represents the channel (see reg3)
        uint8_t mode3k : 1;       //!< (5)   - 1 = 3K; 0 = 5K
        uint8_t ref_32k_mode : 1; //!< (6)   - 1 = 32K ref. crystal clock; 0 = 12MHz ref crystal clock
        uint8_t rsv : 1;          //!< (7)   - Reserved - Debug use, do not change this value using
    } refined;
    uint8_t raw;
} akc595x_reg2;

/**
 * @ingroup GA01
 * @brief Reg3 (type 0x03 / RW): configures register 3 (default: 0xC8) Address
 */
typedef uint8_t akc595x_reg3; //!< 8 least significant bits representing the channel value (see reg2)

/**
 * @ingroup GA01
 * @brief Reg4 (type 0x04 / RW): configures register 4 (default: 0x19) Address
 */
typedef uint8_t akc595x_reg4; //!< Custom channel band start number chan = 32 * usr_chan_start

/**
 * @ingroup GA01
 * @brief Reg5 (type 0x05 / RW): configures register 5 (default: 0x32) Address
 *
 */
typedef uint8_t akc595x_reg5; //!< Custom end-band channel number chan = 32 * usr_chan_stop

/**
 * @ingroup GA01
 * @brief Reg6 (type 0x06 / RW): configures register 0 (default: 0xA1) Address
 */
typedef union {
    struct
    {
        uint8_t phase_inv : 1; //!< 0 = audio output inphase, 1 = opposite phase, for single speaker
        uint8_t line : 1;      //!< 0 = Radio mode; 1 = Line Mode
        uint8_t volume : 6;    //!< Volume: 0 ~ 63 (<24 =  mute; 24 ~ 63)
    } refined;
    uint8_t raw;
} akc595x_reg6;

/**
 * @ingroup GA01
 * @brief Reg7 (type 0x07 / RW): configures register 0 (default: 0xA1) Address
 * @details stereo and mono: "00" the auto stereo, there Stereo_th control threshold ; "10" long as the pilot is forced stereo "x1" forced mono demodulator
 */
typedef union {
    struct
    {
        uint8_t bw : 2;          //!< 00 = 150K; 01 = 200K; 10 = 50K; 11 = 100K
        uint8_t stereo_mono : 2; //!< "00" the auto stereo, there Stereo_th control threshold ; "10" long as the pilot is forced stereo "x1" forced mono demodulator
        uint8_t bben : 1;        //!< Base boost enable 0; Close bass 1.
        uint8_t de : 1;          //!< De-emphasis mode. 1 = 75 μ s (USA); 0 = 50 μ s (China)
        uint8_t rsv : 2;         //!< Measured using, set to "0" during normal use
    } refined;
    uint8_t raw;
} akc595x_reg7;

/**
 * @ingroup GA01
 * @brief Reg8 (type 0x08 / RW):  configures register 8 (default: 0x58) Address
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t stereo_th : 2; //!< FM stereo demodulation start CNR threshold - 00=4, 01=8, 10=12,11=16
        uint8_t fd_th : 2;     //!<
        uint8_t am_cnr_th : 2; //!< When AM mode, chip sets and lighting sentence carrier to noise ratio threshold
        uint8_t fm_cnr_th : 2; //!< When the FM mode, chip sets and lighting sentence carrier to noise. Ratio limit 00-2dB 01-3dB 10-4dB 11-5dB door
    } refined;
    uint8_t raw;
} akc595x_reg8;

/**
 * @ingroup GA01
 * @brief Reg9 (type 0x09 / RW):  configure register 9 (default: 0x07) Address
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t iv_en : 1;      //!< When low voltage power supply, whether to enter the low-voltage operation mode can
        uint8_t rsv1 : 1;       //!< Reserved. Measured using, do not change this value
        uint8_t osc_en : 1;     //!< Oscillator source selection. 0 = 0-External XO; 1 = 1-Crystal
        uint8_t pd_adc_vol : 1; //!< 0 = volume potentiometer mode
        uint8_t rsv2 : 4;       //!< Reserved. Measured using, do not change this value
    } refined;
    uint8_t raw;
} akc595x_reg9;

/**
 * @ingroup GA01
 * @brief Reg11 (type 0x0B / RW):  configure register 9 (default: 0xE0) Address
 *
 * @details Sets FM seek step.
 * | spece |  step   |
 * | ----- |  ------ |
 * |  00   |  25 kHz |
 * |  01   |  50 kHz |
 * |  10   | 100 kHz |
 * |  11   | 200 kHz |
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t rsv1 : 2;  //!< Reserved. Measured using, do not change this value
        uint8_t space : 2; //!< 00 = 25kHz; 01 = 50kHz; 10 = 100kHz; 11 = 200kHz
        uint8_t rsv2 : 4;  //!< Reserved. Measured using, do not change this value
    } refined;
    uint8_t raw;
} akc595x_reg11;

/**
 * @ingroup GA01
 * @brief Reg12 (type 0x0C / RW):  configure register 11 (default: 0x??) Address
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t rsv2 : 5;   //!< Reserved. Measured using, do not change this value
        uint8_t pd_rx : 1;  //!< 0 = analog and RF; 1 = closed channel
        uint8_t res : 1;    //!<
        uint8_t pd_adc : 1; //!< Signal channel ADC signal path; 0 = Close; 1 =  Open
    } refined;
    uint8_t raw;
} akc595x_reg12;

/**
 * @ingroup GA01
 * @brief Reg13 (type 0x0D / RW):  configure register 11 (default: 0x??) Address
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t rsv3 : 2;    //!< Measured using, do not change this value
        uint8_t vol_pre : 2; //!< Adjusting the output volume of the entire values: 00: 0dB 01: 3.5dB
        uint8_t rsv2 : 2;    //!< Measured using, do not change this value
        uint8_t st_led : 1;  //!< 0 = tund pin is tuned lamp; When 1-FM and non wtmode, tund indicator pin is stereo demodulation, tuning lamp remainder
        uint8_t rsv1 : 1;    //!< Measured using, do not change this value
    } refined;
    uint8_t raw;
} akc595x_reg13;

/**
 * @ingroup GA01
 * @brief Reg20 (type 0x14 / R)
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t readchan : 5; //!< Current Channel number (high 5bit). See Reg21
        uint8_t tuned : 1;    //!< When transferred to station 1, station search software can use this determination bit table
        uint8_t stc : 1;      //!< 0 = Not complete; 1 = Complete during Seek and Tune in the register 0.
        uint8_t st : 1;       //!< ??? 0 to the current situation other FM stereo radio ????
    } refined;
    uint8_t raw;
} akc595x_reg20;

/**
 * @ingroup GA01
 * @brief Reg21 (type 0x15 / R)
 * @details Reg21 is the Current Channel number (low 8bit). See Reg 20.
 *
 */
typedef uint8_t akc595x_reg21;

/**
 * @ingroup GA01
 * @brief Reg22 (type 0x16 / R)
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t cnram : 7;    //!< Carrier to noise ratio of the AM signal format, in dB
        uint8_t mode3k_f : 1; //!< 1 = AM 3K channel spacing; 0 =  AM 5K channel spacing
    } refined;
    uint8_t raw;
} akc595x_reg22;

/**
 * @ingroup GA01
 * @brief Reg23 (type 0x17 / R)
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t cnrfm : 7;  //!< Carrier to noise ratio of the FM signal format, in dB
        uint8_t st_dem : 1; //!< Only the demodulated FM stereo (Stereo ratio is greater than 30%) show only one
    } refined;
    uint8_t raw;
} akc595x_reg23;

/**
 * @ingroup GA01
 * @brief Reg24 (type 0x18 / R)
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t lvmode : 1;      //!< Low voltage maximum volume limit mode indication bit
        uint8_t rsv : 1;         //!< Reserved.
        uint8_t pgalevel_if : 3; //!< RF power control loop gain level, the greater the level, a high gain of approximately
        uint8_t pgalevel_rf : 3; //!< RF power control loop gain level, the greater the level, a high gain of approximately
    } refined;
    uint8_t raw;
} akc595x_reg24;

/**
 * @ingroup GA01
 * @brief Reg25 (type 0x19 / R)
 * @details
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t vbat : 6; //!< 6 bits indicating the supply voltage ( unsigned number )
        uint8_t rsv : 2;  //!< Reserved
    } refined;
    uint8_t raw;
} akc595x_reg25;

/**
 * @ingroup GA01
 * @brief Reg26 (type 0x1A / R)
 * @details When the frequency offset indicated, complement format, greater than 127, 256 can be reduced to normal values, attention here to 1kHz units FM, AM in units of 100Hz
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef uint8_t akc595x_reg26; //!<   When the frequency offset indicated, complement format, greater than 127, 256 can be

/**
 * @ingroup GA01
 * @brief Reg27 (type 0x1B / R)
 * @details Antenna aperture can be calculated using signal levels rssi, pgalevel_rf, pgalevel_if
 * @details FM / SW : Pin (dBuV) = 103 - rssi - 6 * pgalevel_rf - 6 * pgalevel_if
 * @details MW / LW : Pin (dBuV) = 123 - rssi - 6 * pgalevel_rf - 6 * pgalevel_if
 *
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t rssi : 7; //!< Antenna aperture can be calculated using signal levels rssi.
        uint8_t rsv : 1;  //!< Reserved
    } refined;
    uint8_t raw;
} akc595x_reg27;

/**
 * @defgroup GA02 AKC695X Class
 * @brief AKC695X Class
 * @details This class implements all functions that will help you to control the AKC695X devices.
 *
 * @author PU2CLR - Ricardo Lima Caratti
 */

class AKC695X
{

protected:
    int deviceAddress = AKC695X_I2C_ADRESS;
    int resetPin = -1;

    uint8_t volume;                             //!< Store the current volume
    uint16_t currentStep;                       //!< Strore the current step
    uint16_t currentFrequency;                  //!< Store the current frequency
    uint8_t currentMode = 0;                    //!< Store the current mode: 1 = FM; 0 = AM
    uint8_t currentCrystalType = CRYSTAL_32KHz; //!< Store the crystal type used:  0 = 12MHz;   1 = 32.768kHz
    uint8_t currentMode3k = 0;                  //!<  1 =  3K; 0 = AM

    // AM current band information
    uint8_t currentBand = 0;
    uint16_t currentBandMinimumFrequency;
    uint16_t currentBandMaximumFrequency;

    // FM current band information
    uint8_t fmCurrentBand = 0;

public:
    // Low level functions
    void reset();
    void setI2CBusAddress(int deviceAddress);

    void setup(int reset_pin);
    void setup(int reset_pin, uint8_t crystal_type);

    void powerOn(uint8_t fm_en, uint8_t tune, uint8_t mute, uint8_t seek, uint8_t seekup);
    void setRegister(uint8_t reg, uint8_t parameter);
    uint8_t getRegister(uint8_t reg);
    void setCrystalType(uint8_t crystal);

    bool isTuned();
    bool isTuningComplete();

    void setFmEmphasis(uint8_t de);
    void setFmStereoMono(uint8_t value);
    void setFmBandwidth(uint8_t value);

    uint8_t isCurrentModeFM();
    uint16_t getCurrentChannel();
    uint16_t channelToFrequency();

    uint8_t getAmCurrentSpace();
    uint8_t getAmCarrierNoiseRatio();
    bool isFmStereo();
    uint8_t getFmCarrierNoiseRatio();

    void setFM(uint8_t akc695x_fm_band, uint16_t minimum_freq, uint16_t maximum_freq, uint16_t default_frequency, uint8_t default_step);
    void setAM(uint8_t akc695x_am_band, uint16_t minimum_freq, uint16_t maximum_freq, uint16_t default_frequency, uint8_t default_step);

    void setCustomBand(uint16_t minimum_frequency, uint16_t maximum_frequency);

    void setStep(uint8_t step);

    void setFmSeekStep(uint8_t value);
    void seekStation(uint8_t up_down, void (*showFunc)() = NULL);

    void setFrequency(uint16_t frequency);
    uint16_t getFrequency();
    void frequencyUp();
    void frequencyDown();

    void setAudio(uint8_t phase_inv, uint8_t line, uint8_t volume);
    void setAudio();

    void setVolume(uint8_t volume);
    void setVolumeControl(uint8_t type);
    void setVolumeUp();
    void setVolumeDown();
    inline int getVolume() { return this->volume; };

    int getRSSI();
    float getSupplyVoltage();

    inline uint8_t getCurrentMode() { return this->currentMode; };

    /**
     * @ingroup GA03A
     * @brief Sets the current AM channel spacing
     * @details Use this function to change the AM channel spacing
    */
    inline void setMode3k(bool on_off) { this->currentMode3k = on_off;};

    void commitTune();

    void convertToChar(uint16_t value, char *strValue, uint8_t len, uint8_t dot, uint8_t separator, bool remove_leading_zeros = true);
};

#endif // _AKC6955_H
