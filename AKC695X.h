
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
 */

/**
 * @defgroup GA01 Defined Data Types  
 * @brief Defined Data Types
 * @details Some information appears to be inaccurate due to translation problems from Chinese to English.
 * @details The information shown here was extracted from Datasheet:
 * @details AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation.
 * @details Other information seems incomplete even in the original Chinese Datasheet. 
 * @details For example: Reg 10 (0x0A). There is no information about it. The Reg11 and 12 seem wrong  
 */

/**
 * @brief Reg0 (type 0x00 / RW): configures register 0 (default: 0x4c) Address
 * @see  AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 11
 */

typedef struct {
    uint8_t rsv : 2;      //!< Reserved - Debug use, do not change this value using
    uint8_t mute : 1;     //!< 1 - Mute L / R channel 0 - Normal operation
    uint8_t seekup : 1;   //!< Seek direction control bit. 0 = Seek down;  1 = Seek up
    uint8_t seek : 1;     //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes
    uint8_t tune : 1;     //!< 0-> 1 Trigger tune process The STC bit is set high when the tune operation completes.
    uint8_t fm_en : 1;    //!< 1 = FM mode;  0 = AM mode
    uint8_t power_on : 1; //!< 1 = Chip on; 0 = Chip off
} akc595x_reg0;

/**
 * @brief Reg1 (type 0x01 / RW): configures register 1 (default: 0x10) Address
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 * 
 */
typedef struct
{
    uint8_t fmband : 3;   //!< 
    uint8_t amband : 4;   //!<  
}  akc595x_reg1;

/**
 * @brief Reg2 (type 0x02 / RW): configure register 2 (default: 0x4A) Address
 * 
 * @details High channel number 5, the channel number and frequencies related as follows: 
 * @details FM mode: Channel Freq = 25kHz * CHAN + 30MHz AM mode, when 5K channel number pattern:. 
 * @details Channel Freq = 5kHz * CHAN AM mode, 3K channel number pattern. when: Channel Freq = 3kHz * CHAN.
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 12
 * 
 */
typedef struct
{
    uint8_t channel : 5;      //!< (0:4) - 5 most significant bits that represents the channel (see reg3)
    uint8_t mode3k : 1;       //!< (5)   - 1 = 3K; 0 = 5K
    uint8_t ref_37k_mode : 1; //!< (6)   - 1 = 32K ref. crystal clock; 0 = 12MHz ref crystal clock
    uint8_t rsv : 1;          //!< (7)   - Reserved - Debug use, do not change this value using
} akc595x_reg2;

/**
 * @brief Reg3 (type 0x03 / RW): configures register 3 (default: 0xC8) Address
 */
typedef uint8_t akc595x_reg3; //!< 8 least significant bits representing the channel value (see reg2)

typedef union {
    struct {
        akc595x_reg2 reg2;
        akc595x_reg3 reg3;
    } refined;
    uint8_t  raw[2];
    uint16_t word;
} akc595x_reg2_reg3;


/**
 * @brief Reg4 (type 0x04 / RW): configures register 4 (default: 0x19) Address
 */
typedef uint8_t akc595x_reg4; //!< Custom channel band start number chan = 32 * usr_chan_start

/**
 * @brief Reg5 (type 0x05 / RW): configures register 5 (default: 0x32) Address
 * 
 */
typedef uint8_t akc595x_reg5; //!< Custom end-band channel number chan = 32 * usr_chan_stop

/**
 * @brief Reg6 (type 0x06 / RW): configures register 0 (default: 0xA1) Address
 */
typedef union {
    struct {
        uint8_t phase_inv : 1;  //!< 0 = audio output inphase, 1 = speakers for audio output, push the two inverted for pushing a speaker
        uint8_t line : 1;       //!< 0 = line input mode; 1 = radio mode
        uint8_t volume : 6;     //!< Volume: 0 ~ 63 ( <24 =  mute; 24 ~ 63)
    }
} akc595x_reg6;

/**
 * @brief Reg7 (type 0x07 / RW): configures register 0 (default: 0xA1) Address
 * @details stereo and mono: "00" the auto stereo, there Stereo_th control threshold ; "10" long as the pilot is forced stereo "x1" forced mono demodulator
 */
typedef union {
    struct
    {
        uint8_t bw: 2;            //!< 00 = 150K; 01 = 200K; 10 = 50K; 11 = 100K
        uint8_t stereo_mono : 2;  //!< "00" the auto stereo, there Stereo_th control threshold ; "10" long as the pilot is forced stereo "x1" forced mono demodulator
        uint8_t bben : 1;         //!< Base boost enable 0; Close bass 1.
        uint8_t de : 1;           //!< De-emphasis mode. 1 = 75 μ s (USA); 0 = 50 μ s (China)
        uint8_t rsv : 2;          //!< Measured using, set to "0" during normal use
    }
} akc595x_reg7;

/**
 * @brief Reg8 (type 0x08 / RW):  configures register 8 (default: 0x58) Address
 * @details 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t stereo_th : 2;  //!< FM stereo demodulation start CNR threshold - 00=4, 01=8, 10=12,11=16
        uint8_t fd_th : 2;      //!<
        uint8_t am_cnr_th : 2;  //!< When AM mode, chip sets and lighting sentence carrier to noise ratio threshold
        uint8_t fm_cnr_th : 2;  //!< When the FM mode, chip sets and lighting sentence carrier to noise. Ratio limit 00-2dB 01-3dB 10-4dB 11-5dB door
    }
} akc595x_reg8;

/**
 * @brief Reg9 (type 0x09 / RW):  configure register 9 (default: 0x07) Address
 * @details 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t iv_en : 1;       //!< When low voltage power supply, whether to enter the low-voltage operation mode can
        uint8_t rsv1 : 1;        //!< Reserved. Measured using, do not change this value
        uint8_t osc_en : 1;      //!< Oscillator source selection. 0 = 0-External XO; 1 = 1-Crystal
        uint8_t pd_adc_vol : 1;  //!< 0 = volume potentiometer mode
        uint8_t rsv2 : 4;        //!< Reserved. Measured using, do not change this value
    }
} akc595x_reg9;

/**
 * @todo  Did not find in Datasheet.
 * @brief Reg10 (type 0x0A / RW):  configure register 10 (default: 0x??) Address
 * @details 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t todo1 : 4;       //!< ??????
        uint8_t todo2 : 4;       //!< ??????
    }
} akc595x_reg10;

/**
 * @brief Reg11 (type 0x0B / RW):  configure register 9 (default: 0xE0) Address
 * @details 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 14
 */
typedef union {
    struct
    {
        uint8_t rsv1 : 2;       //!< Reserved. Measured using, do not change this value
        uint8_t space : 2;      //!< 00 = 25KHz; 01 = 50KHz; 10 = 100KHz; 11 = 200KHz 
        uint8_t rsv2 : 4;       //!< Reserved. Measured using, do not change this value
    }
} akc595x_reg11;


/**
 * @brief Reg12 (type 0x0C / RW):  configure register 11 (default: 0x??) Address
 * @details 
 * 
 * @see AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation; page 15
 */
typedef union {
    struct
    {
        uint8_t pd_adc : 1; //!< Signal channel ADC signal path; 0 = Close; 1 =  Open
        uint8_t res : 1;    //!< ?????
        uint8_t pd_rx : 1;  //!< 0 = analog and RF analog and RF ??????
        uint8_t rsv2 : 5;   //!< Reserved. Measured using, do not change this value
    }
} akc595x_reg12;


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