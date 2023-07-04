#include <Wire.h>
#include "AK4619VN.h"

AK4619VN codec(&Wire, AK4619VN_ADDR);

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(200);
  
  codec.begin(4, 16); // SDA, SCL
  
  uint8_t error = 0;
  
  error = codec.setRstState(true); //Set CODEC to reset state for initialization
  if(error){
    Serial.println("Unable to set codec reset state");
  }
  
  error = codec.pwrMgm(true, true, true, true); //Enable PWR of DAC1&2 and ADC1&2
  if(error){
    Serial.println("Unable to set codec PWR");
  }
  
  error = codec.audioFormatSlotLen(AK4619VN::AK_24BIT, AK4619VN::AK_24BIT);
  if(error){
    Serial.println("Unable to set slot lenght");
  }
  
  error = codec.audioFormatMode(AK4619VN::AK_I2S_STEREO);
  if(error){
    Serial.println("Unable to set audio format mode");
  }
  
  error = codec.sysClkSet(AK4619VN::AK_256FS_96KS, false, false);// Set sample rate to 96kHz, BICK Edge fall, SDOPH Slow
  if(error){
    Serial.println("Unable to set system clock mode");
  }
  
  error = codec.inputGain(AK4619VN::AK_INGAIN_0DB, AK4619VN::AK_INGAIN_0DB, AK4619VN::AK_INGAIN_0DB, AK4619VN::AK_INGAIN_0DB); // Set all ADC gains to 0dB
  if(error){
    Serial.println("Unable to set codec input gain");
  }
  
  error = codec.outputGain(false, AK4619VN::AK_DAC1B, AK4619VN::AK_OUTGAIN_0DB); //Set all DAC1 LR gains to 0dB
  if(error){
    Serial.println("Unable to set DAC1 gain");
  }
  
  error = codec.outputGain(false, AK4619VN::AK_DAC2B, AK4619VN::AK_OUTGAIN_0DB);//Set all DAC2 LR gains to 0dB
  if(error){
    
    Serial.println("Unable to set DAC2 gain");
  }
  
  error = codec.inputConf(AK4619VN::AK_IN_SE1, AK4619VN::AK_IN_SE1, AK4619VN::AK_IN_SE1, AK4619VN::AK_IN_SE1);// Input confing to Single ended 1 on both ADCs
  if(error){
    Serial.println("Unable to set DAC input conf");
  }
  
  error = codec.outputConf(AK4619VN::AK_OUT_SDIN2, AK4619VN::AK_OUT_SDIN1); // DAC1 to SDOUT1, DAC2 to SDOUT2
  if(error){
    Serial.println("Unable to set DAC input conf");
  }
  
  error = codec.setRstState(false); //Release reset state
  if(error){
    Serial.println("Unable to clear codec reset state");
  }
  
  delay(100);
  Serial.println("Setup done");

  codec.printRegs(0x0, 21);
  
}

void loop() {


  while(true); // Do nothing

}
