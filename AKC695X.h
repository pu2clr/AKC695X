
#include <Wire.h>

#define DEFAUL_I2C_ADDRESS 0x10
#define FM_CURRENT_MODE 1
#define AM_CURRENT_MODE 0

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
 * | Presets wave band ( 520 ~ 1730 KHz )                                      |   X     |    X    |    X    |
 * | __Preset shortwave band ( 3.2 ~ 21.9 MHz )__                              |         |    X    |    X    |
 * | Preset long-wave band ( 150 ~ 285 KHz )                                   |   X     |    X    |    X    |
 * | Custom support band                                                       |   X     |    X    |    X    |
 * | It supports a wide supply voltage range: 2.0V ~ 4.5V                      |   X     |    X    |    X    |
 * | stand by 32.768KHz & 12MHz Passive mode crystal reference clock and Pin   |   X     |    X    |    X    |
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
 * 
 * 
 */

/**
 * @brief Reg0: configure register 0 (default: 0x4c) Address
 * 
 */
typedef union {

    struct {
        uint8_t rsv : 2;        //!< Debug use, do not change this value using
        uint8_t mute: 1;        //!< 1 - Mute L / R channel 0 - Normal operation
        uint8_t seekup : 1;     //!< Seek direction control bit. 0 = Seek down;  1 = Seek up
        uint8_t seek : 1;       //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes
        uint8_t tune : 1;       //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes.
        uint8_t fm_en : 1;      //!< 1 = FM mode;  0 = AM mode
        uint8_t power_on : 1;   //!< 1 = Chip on; 0 = Chip off
    } refined;

    uint8_t raw; 

} akc595x_reg0;

/**
 * @brief Reg1: configure register 1 (default: 0x10) Address
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 * 
 */
typedef struct
{
    struct
    {
        uint8_t fmband : 3;   //!< 
        uint8_t amband : 4;   //!<  
    } refined;

    uint8_t raw;

} akc595x_reg1;

typedef struct
{

} akc595x_reg2;

typedef struct
{

} akc595x_reg3;


class AKC695X
{

protected:
    int deviceAddress = DEFAUL_I2C_ADDRESS;
    int step;
    uint8_t currentMode;

public:
    uint8_t setRegister(uint8_t reg, uint8_t parameter);
    uint8_t getRegister(uint8_t reg);

    void setFM();
    void setFM(uint16_t minimalFreq, uint16_t maximumFreq, uint16_t step);
    void setAM(uint8_t band);
    void setAM(uint16_t minimalFreq, uint16_t maximumFreq, uint16_t step);
    void setStep(int step);
    void setFrequency(uint16_t frequency);
    void frequencyUp();
    void frequencyDown();
    void powerUp(); 
};