#include <AKC695X.h>


/**
 * @brief Receiver startup 
 * @todo
 * @param resetPin 
 */
AKC695X::setup(uint8_t resetPin) {

}


/**
 * @brief Sets a given register with a given value 
 * @details It is a basic function to deal with the AKC695X devices 
 * @param reg  register number to be written (only for RW type registers) 
 * @param parameter  value to be written in the register 
 */
    uint8_t
    AKC695X::setRegister(uint8_t reg, uint8_t parameter)
{
    Wire.beginTransmission(deviceAddress);
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
    Wire.beginTransmission(deviceAddress);
    Wire.write(reg);
    Wire.endTransmission();
    delayMicroseconds(3000);
    Wire.requestFrom(device_address, 1);
    
    return  Wire.read();
}

void AKC695X::setFM() {


}

void AKC695X::setFM(uint16_t minimalFreq, uint16_t maximumFreq, uint16_t step){


}

void AKC695X::setAM(uint8_t band) {

}

void AKC695X::setAM(uint16_t minimalFreq, uint16_t maximumFreq, uint16_t step) {

}

void AKC695X::setStep(int step) {


}

void AKC695X::setFrequency(uint16_t frequency) {


}

void AKC695X::frequencyUp() {


}


void AKC695X::frequencyDown() {


}

void AKC695X::powerUp() {


}
