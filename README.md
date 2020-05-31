# PU2CLR AKC695X Arduino Library

This is an Arduino Library to control the AKC695X / M695X DSP radio devices.   

This library can be freely distributed using the MIT Free Software model. 

[Copyright (c) 2020 Ricardo Lima Caratti](https://pu2clr.github.io/AKC695X/#mit-license). 

Contact: __pu2clr@gmail.com__.


# Contents

1. [Preface](https://github.com/pu2clr/AKC695X#preface) 
2. [MIT License](https://pu2clr.github.io/AKC695X/#mit-license)
3. [About AKC695X / M695X DSP radio](https://github.com/pu2clr/AKC695X#about-akc695x--m695x-dsp-radio)
4. [AKC695X Pin definitions](https://github.com/pu2clr/AKC695X#akc695x-pin-definitions)
5. [Schematic](https://github.com/pu2clr/AKC695X#schematic)
6. [API Documentation](https://pu2clr.github.io/AKC695X/extras/docs/html/modules.html)
7. [Examples](https://github.com/pu2clr/AKC695X/tree/master/examples)
8. [Videos](https://github.com/pu2clr/AKC695X#svideos)


## Preface

The AKC695X devices will surprise hobbyists and experimenters with its simplicity to build FM and AM receiver based on AKC695X. There are currently several receivers based on DSP technology. The KT0915 and SI4745 devices are some examples widely disseminated in the market. This library was developed with the purpose of expanding the alternatives to electronics hobbyists and radio listeners. 
In this document you will see Arduino source codes, schematics, examples and tips to help you to build a receiver based on Arduino board and AKC695X devices. 


## MIT License 

Copyright (c) 2019 Ricardo Lima Caratti

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE ARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## About AKC695X / M695X DSP radio 

The AKC695X is a family of IC DSP receiver from AKC technology. The AKC6955 and AKC6959 support AM and FM modes. 
On AM mode the AKC6955 and AKC6969 work on LW, MW and SW. On FM mode they work from 64MHz to 222MHz.


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


### Register operations
   
This library uses the I2C protocols to read and write AKC695X registers. In this context, registers are memory position into the device.

The first 13 registers, you can use to change the behavior of the device. They are read and write registers. The registers from 20 to 27, can be used to get the current device status. They are read only registers. 

There is no information about the register 10 and the registers 14 to 19 in the documentation used to develop this library. 

By using the registers 0 to 13, you can change the band, set the frequency, set the channel space, set the audio behavior and volume, set a custom band and more. The file [AKC695X.h](https://github.com/pu2clr/AKC695X/blob/master/AKC695X.h) has details about the all registers used in the library. Also, you can read the [API Documentation](https://pu2clr.github.io/AKC695X/extras/docs/html/modules.html) to know more about AKC695X registers. 


You can use the registers 0, 1, 2 and 3 to make the receiver work with the most important functionalities. these registers are described below.


### Table Reg1

Reg1: configure register 1 (default: 0x10) Address - Type 0x01 (RW)

|  BIT  |  Label    |  Default | Function Description | 
| ----- | --------- | -------- | -------------------- |         
| 7:3   | amband    |  0x2     | see table [Table Reg1 amband]() | 
| 2:0   | fmband    |  0x00    | see table [Table Reg1 fmband]() |           

Band seek logic chip only valid for tune logic, frequency can be adjusted at any stage


#### Table Reg1 amband

The table below can help you to select the right band and its frequency limits. You might need to use it in your Arduino sketch.

| amband value | N#  |Description  |
| ------------ | --- |------------ |
| 00000        |  0  |LW, 0.15 ~ 0.285, 3K station search |
| 00001        |  1  |MW1, 0.52 ~ 1.71, 5K station search |
| 00010        |  2  |MW2, 0.522 ~ 1.62, 9K station search |
| 00011        |  3  |MW3, 0.52 ~ 1.71, 10K station search |
| 00100        |  4  |SW1, 4.7 ~ 10, 5K station search |
| 00101        |  5  |SW2, 3.2 ~ 4.1, 5K station search |
| 00110        |  6  |SW3, 4.7 ~ 5.6, 5K station search |
| 00111        |  7  |SW4, 5.7 ~ 6.4, 5K station search |
| 01000        |  8  |SW5, 6.8 ~ 7.6, 5K station search |
| 01001        |  9  |SW6, 9.2 ~ 10, 5K station search |
| 01010        | 10  |SW7, 11.4 ~ 12.2, 5K station search |
| 01011        | 11  |SW8, 13.5 ~ 14.3 |
| 01100        | 12  |SW9, 15 ~ 15.9 |
| 01101        | 13  |SW10, 17.4 ~ 17.9 |
| 01110        | 14  |SW11, 18.9 ~ 19.7, 5K station search |
| 01111        | 15  |SW12, 21.4 ~ 21.9, 5K station search |
| 10000        | 16  |SW13, 11.4 ~ 17.9, 5K station search |
| 10010        | 17  |MW4, 0.52 to 1.73, 5K station search |
| Other        | 18+ |custom band, station search interval = 3K |


#### Table Reg1 fmband

| fmband value | N#  |Description  |
| ------------ | --- |------------ |
| 000          |  0  | FM1,87 ~ 108, station search space specified intervals |
| 001          |  1  | FM2,76 ~ 108, station search space specified intervals |
| 010          |  2  | FM3,70 ~ 93, with a space station search interval set |
| 011          |  3  | FM4,76 ~ 90, Tuning predetermined space intervals |
| 100          |  4  | FM5,64 ~ 88, with a space station search interval set | 
| 101          |  5  | TV1,56.25 ~ 91.75, station search space specified intervals |
| 110          |  6  | TV2, 174.75 ~ 222.25, found |
| 111          |  7  | sets predetermined space intervals, custom FM, station search space specified intervals |



### Table Reg2  and Reg3

The registers 2 and 3 are used together. The tuning frequency is obtained by calculation. The formula is described below.

#### Reg2

|  BIT  |  Label     |  Default | Function Description | 
| ----- | ---------  | -------- | -------------------- |         
| 7     |  rsv       |    0     | Reserved for internal use.      | 
| 6     | ref_32k_mo |    1     | 1 = 32.768 crystal; 0 = 12MHz crystal  |
| 5     | Mode3k     |    0     | 1 = 3K custom channel number as the AM mode; 0 = custom channel number pattern 5K |
| 4:0   | Channel    |   0x0A   | The higher 5 bits of the channel number. See comments [ˆ1] and [ˆ2] |


#### Reg3

|  BIT  |  Label     |  Default | Function Description | 
| ----- | ---------  | -------- | -------------------- |         
| 7:0   |  rsv       |   0xC8   | The lower 8 bits of the channel number.  See comment [ˆ1]  | 



[ˆ1]  
1. __On FM mode: Channel Freq = 25kHz * CHAN + 30MHz__; 
2. __On AM mode__: 
   * when 5K channel number pattern, Channel Freq = 5kHz * CHAN 
   * when 3K channel number pattern, Channel Freq = 3kHz * CHAN. 

[ˆ2] __If the MCU is working with MW2 (see table Table Reg1 amband), the channel number has to be a multiple of three. Otherwise, the radio will be a mess.__  


## Schematic 

The figure below shows the basic schematic of the AKC695X and Arduino Pro Mini 3.3V, 8MHz.  

![Basic Schematic](https://github.com/pu2clr/AKC695X/blob/master/extras/images/basic_schematic.png)


### AKC695X Pin definitions

The figure and table below show the pin description of the AKC6951 and AKC6955.

![AKC6951(55) pin out](https://github.com/pu2clr/AKC695X/blob/master/extras/images/AKC695X_pinout.png)


| Pin | Name      | Description  | 
| --- | --------- | ------------ | 
| 1   | oscin     | Bonding or passive 32.768K 12MHz crystal to ground, or receive an external clock reference signal |
| 2   | oscout    | Passive other end connected to the crystal, when connected to an external clock, this pin floating |
| 3   | gnd       | Close to ground | 
| 4   | tund      | Radio lock indicator pin, connected directly to the light emitting diode; Tuning the MCU software when the pin may also be used as a stop sign | 
| 5   | p_on      | On-chip power switch, high input power chip; the chip down in a low-power standby state, power consumption of approximately 10uA |
| 6   | sclk      | 2C clock signal input | 
| 7   | sdio      | I2C bidirectional data signal input / output | 
| 8   | inl       | External audio signal input L, proposes to add blocking capacitor 1uF | 
| 9   | inr       | External audio input signal R, proposes to add blocking capacitor 1uF |
| 10  | vbat      | Then the power pins need to pay attention to the nearest ground 0.1uF decoupling capacitance to ground | 
| 11  | lout      | Left channel audio output | 
| 12  | gnd       | gnd | 
| 13  | rout      | Right channel audio output | 
| 14  | vref      | Precision 1.5V output pin, to provide a baseline volume potentiometer | 
| 15  | vol       | Variable volume potentiometer connected end, the fixed end of a potentiometer directly connected to VREF, and the other end through a resistor to ground. The resistance ratio of the resistor and potentiometer as 1: 2 |
| 16  | vbat      | Then the power pins need to pay attention to the nearest ground 0.1uF decoupling capacitance to ground |
| 17  | gnd       | Close to ground | 
| 18  | mwinN     | MW and LW differential input signal |
| 19  | mwinP     | MW and LW differential input signal |
| 20  | swin      | Shortwave signal input terminal, note add blocking capacitor recommended 3nF | 
| 21  | gnd       | Close to ground | 
| 22  | fmin      | FM radio frequency signal input terminal, note add blocking capacitor, 100pF recommendations | 
| 23  | ldocap    | Internal LDO output pin, nearest the need decoupling capacitors, recommendations 47uF |      
| 24  | gnd       | Close to ground | 



# Receivers based on

* [Troy reviews the Audiomax SRW-710S](https://swling.com/blog/tag/shortwave-radio-review/)




# References

* [AM-SW-FM radio by DSP radio chip version 2](https://www.tindie.com/products/microwavemont/am-sw-fm-radio-by-dsp-radio-chip-version-2/)
* [Radio, yes AM-SW-FM radio by DSP radio chip](https://hackaday.io/project/12944-radio-yes-am-sw-fm-radio-by-dsp-radio-chip)
*


# Videos

* [https://youtu.be/BHW2wCZiTkU](https://youtu.be/BHW2wCZiTkU)
* [PU2CLR AKC695X Arduino Library - SEEKING TEST](https://youtu.be/3OwnVBmOjAs)



