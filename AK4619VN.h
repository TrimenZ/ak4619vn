#ifndef AK4619VN_H
#define AK4619VN_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"

#define AK4619VN_ADDR 0x10
#define AK4619VN_ALT_ADDR 0x12

#define PWRMGM 0x0 //
#define AUDFORM1 0x01 //
#define AUDFORM2 0x02 //
#define SYSCLKSET 0x03 //
#define MICGAIN1 0x04 //
#define MICGAIN2 0x05 //
#define ADC1LVOL 0x06 //
#define ADC1RVOL 0x07 //
#define ADC2LVOL 0x08 //
#define ADC2RVOL 0x09 //
#define ADCFILT 0x0A 
#define ADCAIN 0x0B //
#define ADCMUTEHPF 0x0D
#define DAC1LVOL 0x0E //
#define DAC1RVOL 0x0F //
#define DAC2LVOL 0x10 //
#define DAC2RVOL 0x11 //
#define DACDIN 0x12 //
#define DACDEEM 0x13
#define DACMUTFLT 0x14

class AK4619VN {
public:
    AK4619VN(TwoWire *i2c, uint8_t i2cAddress = AK4619VN_ADDR); // Constructor
    void begin(uint8_t SDA, uint8_t SCL);
    void begin(void);
    
     uint8_t setRstState(bool state);
     uint8_t pwrMgm(bool ADC2, bool ADC1, bool DAC2, bool DAC1);
     
//###############
    typedef enum{
        AK_24BIT = 0x0,
        AK_20BIT = 0x01,
        AK_16BIT = 0x02,
        AK_32BIT = 0x03,
    } data_bit_length_t;
    
    //Bit order TDM, DCF[2:0], DSL[1:0], SLOT
    typedef enum{
        AK_I2S_STEREO = 0x0,
        AK_MSB_STEREO = 0x28,
        AK_PCM_SHRT_16B = 0x35,
        AK_PCM_SHRT_24B = 0x31,
        AK_PCM_SHRT_32B = 0x37,
        AK_PCM_LONG_16B = 0x3D,
        AK_PCM_LONG_24B = 0x39,
        AK_PCM_LONG_32B = 0x3F,
        AK_TDM128_I2S_32B = 0x57,
        AK_TDM128_MSB_32B = 0x7F,
        AK_TDM256_I2S_32B = 0x57,
    } audio_iface_format_t;
    
    typedef enum{
        AK_256FS_8_48KS = 0x0,
        AK_256FS_96KS = 0x01,
        AK_384FS_8_48KS = 0x02,
        AK_512FS_8_48KS = 0x03,
        AK_128FS_192KS = 0x04,
    } clk_fs_t;
    
    
    uint8_t audioFormatSlotLen(data_bit_length_t IDL, data_bit_length_t ODL);
    uint8_t audioFormatMode(audio_iface_format_t FORMAT);
    uint8_t sysClkSet(clk_fs_t FS, bool BICKEdg, bool SDOPH);
//###############    
    typedef enum{
        AK_INGAIN_N6DB = 0x0,
        AK_INGAIN_N3DB = 0x01,
        AK_INGAIN_0DB = 0x02,
        AK_INGAIN_3DB = 0x03,
        AK_INGAIN_6DB = 0x04,
        AK_INGAIN_9DB = 0x05,
        AK_INGAIN_12DB = 0x06,
        AK_INGAIN_15DB = 0x07,
        AK_INGAIN_18DB = 0x08,
        AK_INGAIN_21DB = 0x09,
        AK_INGAIN_24DB = 0x0A,
        AK_INGAIN_27DB = 0x0B,
    } input_gain_t;
    
    uint8_t inputGain(input_gain_t ADC1L, input_gain_t ADC1R, input_gain_t ADC2L, input_gain_t ADC2R);
    
//###############
    typedef enum{
        AK_DAC1B = 0x0,
        AK_DAC2B = 0x01,
        AK_DAC1L = 0x02,
        AK_DAC1R = 0x03,
        AK_DAC2L = 0x04,
        AK_DAC2R = 0x05,
        AK_OUTGAIN_0DB = 0x30,
        AK_OUT_GAIN_MUTE = 0xFF,
    } output_gain_t;
        
    uint8_t outputGain(bool relative, output_gain_t channel, int16_t gainVal);
    
//###############
    typedef enum{
        AK_IN_DIFF = 0x0,
        AK_IN_SE1 = 0x01,
        AK_IN_SE2 = 0x02,
        AK_IN_PSDIFF = 0x02,
    } intput_conf_t;
    
    uint8_t inputConf(intput_conf_t ADC1L, intput_conf_t ADC1R, intput_conf_t ADC2L, intput_conf_t ADC2R);

//###############
    typedef enum{
        AK_OUT_SDIN1 = 0x0,
        AK_OUT_SDIN2 = 0x01,
        AK_OUT_SDOUT1 = 0x02,
        AK_OUT_SDOUT2 = 0x03,
    } output_conf_t;
    
    uint8_t outputConf(output_conf_t DAC2, output_conf_t DAC1);
    
    uint8_t printRegs(uint8_t startReg, uint8_t len);
    
    
    
private:
    TwoWire *_wirePort;
    uint8_t _i2cAddress;
   
#if ESP_ARDUINO_VERSION_MAJOR == 1
    //For Arduino-ESP32 v1+
    uint8_t writeRegOld(uint8_t deviceReg, uint8_t regVal);
    uint8_t readRegOld(uint8_t deviceReg, uint8_t * regVal);
    uint8_t readRegMultiOld(uint8_t startReg, uint8_t len, uint8_t * vals);
#endif

    
#if ESP_ARDUINO_VERSION_MAJOR == 2 || not defined ESP_ARDUINO_VERSION_MAJOR
    //For Arduino-ESP32 v2+
    //Use as default for non-ESP32 platforms
    uint8_t writeReg(uint8_t deviceReg, uint8_t regVal);
    uint8_t readReg(uint8_t deviceReg, uint8_t * regVal);
    uint8_t readRegMulti(uint8_t startReg, uint8_t len, uint8_t * vals);
#endif
    
    uint8_t modifyGainRange(int16_t inVal, uint8_t regVal);
    uint8_t checkGainRange(int16_t inVal);
    TwoWire *m_i2c;
    uint8_t m_addr;    
};

#endif // AK4619VN_H
