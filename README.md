# AKC695X

__ATTENTION__: this library is under construction..... 

This is an Arduino Library to control the AKC695X / M695X DSP radio devices. 


Author: Ricardo Lima Caratti, 2020. 


## About AKC695X / M695X DSP radio 

### AKC695X features

The table below shows some features fo the AKC695X devices family.


| Feature                                                                   | AKC6951 | AKC6955 | AKC6959 |
| ------------------------------------------------------------------------- | ------- | ------- | ------- |
| stand by 30 ~ 230MHz Any frequency FM demodulation                        |   X     |    X    |    X    |
| stand by 0.15 ~ 30MHz Any frequency AM demodulation                       |   X     |    X    |    X    |
| Preset FM band ( 64 ~ 108 MHz )                                           |   X     |    X    |    X    |
| stand by TV1 audio( 56.25 ~ 91.75 MHz )                                   |   X     |    X    |    X    |
| stand by TV2 audio( 174.75 ~ 222.25 MHz )                                 |   X     |    X    |    X    |
| Presets wave band ( 520 ~ 1730 KHz )                                      |   X     |    X    |    X    |
| Preset shortwave band ( 3.2 ~ 21.9 MHz )                                  |         |    X    |    X    |
| Preset long-wave band ( 150 ~ 285 KHz )                                   |   X     |    X    |    X    |
| Custom support band                                                       |   X     |    X    |    X    |
| It supports a wide supply voltage range: 2.0V ~ 4.5V                      |   X     |    X    |    X    |
| stand by 32.768KHz & 12MHz Passive mode crystal reference clock and Pin   |   X     |    X    |    X    |
| Integrated audio amplifier (maximum power differential 0.5W )             |   X     |    X    |    X    |
| Two kinds of control volume: volume potentiometer and the volume register |   X     |    X    |    X    |
| Two kinds of de-emphasis modes: 50us / 75us                               |   X     |    X    |    X    |
| Support tuning lamp function                                              |   X     |    X    |    X    |
| The audio output may be in phase, may be inverted output                  |   X     |    X    |    X    |
| Support for stereo line input                                             |   X     |    X    |    X    |
| Support low-power standby mode, 3V When power consumption 10uA            |   X     |    X    |    X    |
| Internal integrated PLL                                                   |   X     |    X    |    X    |
| Intelligent frequency control                                             |   X     |    X    |    X    |
| AGC                                                                       |   X     |    X    |    X    |
| Precise digital demodulation                                              |   X     |    X    |    X    |
| Smart mute function                                                       |   X     |    X    |    X    |
| SW Increased tracking filter, greatly improving SW Audibility             |         |    X    |    X    |
| MW Precise tuning adaptive front end                                      |   X     |    X    |    X    |
| FM Subwoofer                                                              |   X     |    X    |    X    |
| integrated LDO                                                            |   X     |    X    |    X    |
| According to the battery voltage, automatically adjust the volume         |   X     |    X    |    X    |
| Pb-free / RoHS complian                                                   |   X     |    X    |    X    |

__Sources:__  AKC6951, AKC6955 and AKC6959 Datasheets.


## Registers setup

### Table Reg1

Reg1: configure register 1 (default: 0x10) Address - Type 0x01 (RW)

|  BIT  |  Label    |  Default | Function Description | 
| ----- | --------- | -------- | -------------------- |         
| 7:3   | amband    |  0x2     | see table [Table Reg1 amband]() | 
| 2:0   | fmband    |  0x00    | see table [Table Reg1 fmband]() |           

Band seek logic chip only valid for tune logic, frequency can be adjusted at any stage


#### Table Reg1 amband

| amband value | Description  |
| ------------ | ------------ |
| 00000        | LW, 0.15 ~ 0.285, 3K station search |
| 00001        | MW1, 0.52 ~ 1.71, 5K station search |
| 00010        | MW2, 0.522 ~ 1.62, 9K station search |
| 00011        | MW3, 0.52 ~ 1.71, 10K station search |
| 00100        | SW1, 4.7 ~ 10, 5K station search |
| 00101        | SW2, 3.2 ~ 4.1, 5K station search |
| 00110        | SW3, 4.7 ~ 5.6, 5K station search |
| 00111        | SW4, 5.7 ~ 6.4, 5K station search |
| 01000        | SW5, 6.8 ~ 7.6, 5K station search |
| 01001        | SW6, 9.2 ~ 10, 5K station search |
| 01010        | SW7, 11.4 ~ 12.2, 5K station search |
| 01011        | SW8, 13.5 ~ 14.3 |
| 01100        | SW9, 15 ~ 15.9 |
| 01101        | SW10, 17.4 ~ 17.9 |
| 01110        | SW11, 18.9 ~ 19.7, 5K station search |
| 01111        | SW12, 21.4 ~ 21.9, 5K station search |
| 10000        | SW13, 11.4 ~ 17.9, 5K station search |
| 10010        | MW4, 0.52 to 1.73, 5K station search |
| Other        | custom band, station search interval = 3K |


#### Table Reg1 fmband

| fmband value | Description  |
| ------------ | ------------ |
| 000          | FM1,87 ~ 108, station search space specified intervals |
| 001          | FM2,76 ~ 108, station search space specified intervals |
| 010          | FM3,70 ~ 93, with a space station search interval set |
| 011          | FM4,76 ~ 90, Tuning predetermined space intervals |
| 100          | FM5,64 ~ 88, with a space station search interval set | 
| 101          | TV1,56.25 ~ 91.75, station search space specified intervals |
| 110          | TV2, 174.75 ~ 222.25, found |
| 111          | sets predetermined space intervals, custom FM, station search space specified intervals |


# Receivers based on

* [Troy reviews the Audiomax SRW-710S](https://swling.com/blog/tag/shortwave-radio-review/)




# References

* [AM-SW-FM radio by DSP radio chip version 2](https://www.tindie.com/products/microwavemont/am-sw-fm-radio-by-dsp-radio-chip-version-2/)
* [Radio, yes AM-SW-FM radio by DSP radio chip](https://hackaday.io/project/12944-radio-yes-am-sw-fm-radio-by-dsp-radio-chip)
*




