#include "AK4619VN.h"

AK4619VN::AK4619VN(TwoWire *i2c, uint8_t i2cAddress) {
    if(i2c == NULL){
        while(1);
    }
    
    m_i2c = i2c;
    m_addr = i2cAddress;
}

void AK4619VN::begin(uint8_t SDA, uint8_t SCL ) {
    m_i2c->begin(SDA, SCL);
}

void AK4619VN::begin(void){
    m_i2c->begin();
}


//Set reset state 
/*
 * Use to set CODEC in reset state.
 * true = reset state
 * false = normal operation
 * CODEC should be configured in reset state, 
 * release it after configuration
 */
uint8_t AK4619VN::setRstState(bool state){
  uint8_t regval = 0;
  uint8_t error = 0;
  
  error = readReg(PWRMGM, &regval);
  if(error){
    return error;
  }
  
  regval |= !state;
  
  return (writeReg(PWRMGM, regval));
}

//Power MGM of codec DAC&ADC
uint8_t AK4619VN::pwrMgm(bool ADC2, bool ADC1, bool DAC2, bool DAC1){
  uint8_t regval = 0;
  uint8_t rstState = 0;
  uint8_t error = 0;
  
  regval = (ADC2 << 5 | ADC1 << 4 | DAC2 << 2 | DAC1 << 1);
  
  //Check for RESET state of CODEC
  error = readReg(PWRMGM, &rstState);
  if(error){
    Serial.println(error, DEC);
    Serial.println("Unable to read PWR reg");
    return error;
  }
  
  if( !((rstState >> 0) & 1) ){
    Serial.println("Codec is in reset state");
  }
  
  return (writeReg(PWRMGM, regval));
  
}

// //Set audio format, I&O data length and MCLK and sample freq
// /*
//  * slot = 0 - LRCK edge | 1 - Slot length
//  * IDL:
//  * 00 - 24bit
//  * 01 - 20bit
//  * 10 - 16bit
//  * 11 - 32bit
//  * 
//  * ODL:
//  * Samle as IDL except
//  * 11 - N/A
//  * 
//  * FS:
//  * bits   MCLK    fs range
//  * 000 - 256fs    8 kHz ≦ fs ≦ 48 kHz
//  * 001 - 256fs    fs = 96 kHz
//  * 010 - 384fs    8 kHz ≦ fs ≦ 48 kHz
//  * 011 - 512fs    8 kHz ≦ fs ≦ 48 kHz
//  * 1xx - 128fs    fs = 192 kHz
//  * ** For all FS BICK in range 32, 48, 64, 128, 256fs
//  * ** except 1xx which exclude 256fs
//  */
// 
// uint8_t AK4619VN::audioFormatClk(bool TDM, bool slot, data_bit_length_t IDL, data_bit_length_t ODL){
//   
//   uint8_t regval0 = 0;
//   uint8_t regval1 = 0;
//   uint8_t error = 0;
//   
//   if(ODL == AK_32BIT){
//     Serial.println("Invalid ODL settings");
//   }
//   
//   regval0 = (slot <<4 | IDL << 3 | ODL);
//   regval1 = FS;
//   
//   error = writeReg(AUDFORM1, regval0);
//   if(error){
//     return error;
//   }
//   
//   return (writeReg(AUDFORM2, regval1));
//   
// }


uint8_t AK4619VN::audioFormatSlotLen(data_bit_length_t IDL, data_bit_length_t ODL){
  
  uint8_t regval = 0;
  uint8_t error = 0;
  
  error = readReg(AUDFORM2, &regval);
  if(error){
    return error;
  }
  
  uint8_t tempval = (IDL << 2 | ODL);
  
  regval &= 0xF0;
  regval |= tempval;

  
  return (writeReg(AUDFORM2, regval));
  
}

//Set audio format mode 
uint8_t AK4619VN::audioFormatMode(audio_iface_format_t FORMAT){
  
  uint8_t regval0 = 0;
  uint8_t regval1 = 0;
  uint8_t error = 0;
  
  error = readReg(AUDFORM1, &regval0);
  if(error){
    return error;
  }
  
  error = readReg(AUDFORM2, &regval1);
  if(error){
    return error;
  }
  
  //Clear upper 6 bits, shift FORMAT by 2 and set it
  regval0 &= 0x03;
  int temp = FORMAT << 2;
  regval0 |= temp;
  
  regval1 &= ~(0x08); //Mask 4th bit and NOT it
  int tempval = (FORMAT & 0x01); // Set tempval to 1st bit of FORMAT
  regval1 |= (tempval << 3); // Set 4th bit to tempval
  
  error = writeReg(AUDFORM1, regval0);
  if(error){
    return error;
  }
  
  return (writeReg(AUDFORM2, regval1));
  
}

uint8_t AK4619VN::sysClkSet(clk_fs_t FS, bool BICKEdg, bool SDOPH){
  
  uint8_t regval = 0;
  uint8_t error = 0;
  
  error = writeReg(SYSCLKSET, FS);
  if(error){
    return error;
  }
  
  
  uint8_t tempval  = (BICKEdg << 1 | SDOPH);
  
  error = readReg(AUDFORM1, &regval);
  if(error){
    return error;
  }
  
  regval &= 0xFC;
  regval |= (tempval & 0x03);
  
  return (writeReg(AUDFORM1, regval));
}


//Set ADC input gain
uint8_t AK4619VN::inputGain(input_gain_t ADC1L, input_gain_t ADC1R, input_gain_t ADC2L, input_gain_t ADC2R){
  
  uint8_t regval0 = 0;
  uint8_t regval1 = 0;
  uint8_t error = 0;
  
  regval0 = (ADC1L << 4 | ADC1R);
  regval1 = (ADC2L << 4 | ADC2R);
  
  error = writeReg(MICGAIN1, regval0);
  if(error){
    return error;
  }
  
  return (writeReg(MICGAIN2, regval1));
  
}

//Set DAC output gain
/*
 * relative - change gain relatively by gainVal
 * channel -  DACxB - change LR
 *            DACxL/RL - change only one
 * gainVal -  0x00 - MAX +24dB
 *            0xFF - Muted
 *            Steps in 0.5dB
 *            0dB = 0x30 - CODEC default
 */

uint8_t AK4619VN::outputGain(bool relative, output_gain_t channel, int16_t gainVal){
  
  uint8_t regvals[4] = {0};
  uint8_t regval = 0;
  uint8_t error = 0;
    
  if(relative){
    error = readRegMulti(DAC1LVOL, 4, regvals); //Read output gain regvals
    Serial.printf("Regvals:\t %i, %i, %i, %i\n\r", regvals[0], regvals[1], regvals[2], regvals[3]);
    
    if(error){
      return error;
    }
    
    switch(channel){
      case AK4619VN::AK_DAC1B :
        
        regvals[0] = modifyGainRange(gainVal, regvals[0]);
        regvals[1] = modifyGainRange(gainVal, regvals[1]);
        
        error = writeReg(DAC1LVOL, regvals[0]);
        if(error){
          return error;
        }
        
        error = writeReg(DAC1RVOL, regvals[1]);
        if(error){
          return error;
        }
        break;
        
      case AK4619VN::AK_DAC2B :
        regvals[2] = modifyGainRange(gainVal, regvals[2]);
        regvals[3] = modifyGainRange(gainVal, regvals[3]);
        
        error = writeReg(DAC2LVOL, regvals[2]);
        if(error){
          return error;
        }
        
        error = writeReg(DAC2RVOL, regvals[3]);
        if(error){
          return error;
        }
        break; 
        
      case AK4619VN::AK_DAC1L :
        regvals[0] = modifyGainRange(gainVal, regvals[0]);
        return (writeReg(DAC1LVOL, regvals[0]));
        break;
        
      case AK4619VN::AK_DAC1R :
        regvals[1] = modifyGainRange(gainVal, regvals[1]);
        return (writeReg(DAC1RVOL, regvals[1]));
        break;
        
      case AK4619VN::AK_DAC2L :
        regvals[2] = modifyGainRange(gainVal, regvals[2]);
        return (writeReg(DAC2LVOL, regvals[2]));
        break;
        
      case AK4619VN::AK_DAC2R :
        regvals[3] = modifyGainRange(gainVal, regvals[3]);
        return (writeReg(DAC2RVOL, regvals[3]));
        break;
        
      default:
        break;
    }
  }
  else{
    switch(channel){
      case AK4619VN::AK_DAC1B :
        regvals[0] = checkGainRange(gainVal);
        regvals[1] = checkGainRange(gainVal);
        
        error = writeReg(DAC1LVOL, regvals[0]);
        if(error){
          Serial.print("DAC1LVOL Err: ");
          Serial.println(error, DEC);
          return error;
        }
        
        error = writeReg(DAC1RVOL, regvals[1]);
        if(error){
          Serial.print("DAC1RVOL Err: ");
          Serial.println(error, DEC);
          return error;
        }
        break;
        
      case AK4619VN::AK_DAC2B :
        regvals[2] = checkGainRange(gainVal);
        regvals[3] = checkGainRange(gainVal);
        
        error = writeReg(DAC2LVOL, regvals[2]);
        if(error){
          Serial.print("DAC2LVOL Err: ");
          Serial.println(error, DEC);
          return error;
        }
        
        error = writeReg(DAC2RVOL, regvals[3]);
        if(error){
          Serial.print("DAC2RVOL Err: ");
          Serial.println(error, DEC);
          return error;
        }
        break;
        
      case AK4619VN::AK_DAC1L :
        regvals[0] = checkGainRange(gainVal);
        return (writeReg(DAC1LVOL, regvals[0]));
        break;
        
      case AK4619VN::AK_DAC1R :
        regvals[1] = checkGainRange(gainVal);
        return (writeReg(DAC1RVOL, regvals[1]));
        break;
        
      case AK4619VN::AK_DAC2L :
        regvals[2] = checkGainRange(gainVal);
        return (writeReg(DAC2LVOL, regvals[2]));
        break;
        
      case AK4619VN::AK_DAC2R :
        regvals[3] = checkGainRange(gainVal);
        return (writeReg(DAC2RVOL, regvals[3]));
        break;
        
      default:
        break;
    }
  }
  return 0;
}

//ADC input configuration
uint8_t AK4619VN::inputConf(intput_conf_t ADC1L, intput_conf_t ADC1R, intput_conf_t ADC2L, intput_conf_t ADC2R){
  
  uint8_t regval = 0;
  uint8_t error = 0;
  
  regval = ( ADC1L << 6 | ADC1R << 4 | ADC2L << 2 | ADC2R);
  
  return (writeReg(ADCAIN, regval));
}

//DAC input configuration
uint8_t AK4619VN::outputConf(output_conf_t DAC2, output_conf_t DAC1){
  
  uint8_t regval = 0;
  uint8_t error = 0;
  
  regval = ( DAC2 << 2 | DAC1);
  
  return (writeReg(DACDIN, regval));
}

uint8_t AK4619VN::printRegs(uint8_t startReg, uint8_t len){
  uint8_t regvals[128] = {0};
  
  readRegMulti(startReg, len, regvals);
  
  //TODO remove for DBG only
  for(int idx = 0; idx < len; idx++){
    Serial.print(idx, HEX);
    Serial.print(": \t");
    
    for (int8_t i = 7; i >= 0; i--) {
      uint8_t bit = (regvals[idx] >> i) & 1;
      Serial.print(bit);
      if (i % 4 == 0)
        Serial.print(" ");
    }
    Serial.print("\t");
    Serial.print(regvals[idx], HEX);
    Serial.println();  // Prints a new line after printing the bits
  
  }
}
    

uint8_t AK4619VN::writeReg(uint8_t deviceReg, uint8_t regVal) {
    m_i2c->beginTransmission(m_addr);
    m_i2c->write(deviceReg);
    m_i2c->write(regVal);
    m_i2c->endTransmission(true);
    
    return return false;
}

uint8_t AK4619VN::readReg(uint8_t deviceReg, uint8_t * regVal) {
    m_i2c->beginTransmission(m_addr);
    m_i2c->write(deviceReg);
    m_i2c->endTransmission(true);
    
    m_i2c->beginTransmission(m_addr);
    m_i2c->requestFrom((int8_t)m_addr, 1);
    * regVal = m_i2c->read();
    m_i2c->endTransmission(true);
    
    return return false;
}

uint8_t AK4619VN::readRegMulti(uint8_t startReg, uint8_t len, uint8_t * vals) {
    m_i2c->beginTransmission(m_addr);
    m_i2c->write(startReg);
    m_i2c->endTransmission(false);
    
    m_i2c->requestFrom((int8_t)m_addr, (int8_t)len);
    
    for (uint8_t i = 0; i <= (len - 1); i++) {
    vals[i] = m_i2c->read();
  }
    
    return return false;
}


//Modify regVal by inVal, check for under/overflow and adjust regVal to min or max
uint8_t AK4619VN::modifyGainRange(int16_t inVal, uint8_t regVal){
  if (inVal > 0 && (0xFF - regVal) < inVal) {
        // Overflow occurred, return maximum value
        return 0xFF;
    } else if (inVal < 0 && regVal < (-inVal)) {
        // Underflow occurred, return minimum value
        return 0;
    } else {
        // No overflow or underflow, perform the update
        return regVal + inVal;
    }
}

//Check if inVal is in range of uint8_t
uint8_t AK4619VN::checkGainRange(int16_t inVal){
  if (inVal > 0xFF) {
        // Value exceeds the maximum of uint8_t
        return 0xFF;
    } else if (inVal < 0) {
        // Value is negative, return minimum value of uint8_t
        return 0;
    } else {
        // Value is within the range of uint8_t, return as is
        return (uint8_t)inVal;
    }
}
  
