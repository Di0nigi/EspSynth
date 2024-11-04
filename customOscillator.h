#ifndef CUSTOMOSCILLATOR_H  
#define CUSTOMOSCILLATOR_H


#include <Arduino.h>
#include <math.h>


class Oscillator{
  public: 
  float amplitude = 0.5;
  int sampleRate = 8000;
  float phase=0.0;  
  float phaseIncrement=0.0; 
  int frequency = 0;


  int type = 0;
  Oscillator(){};
  
  void setFrequency(int freq) {
        frequency = freq;
        phaseIncrement = static_cast<float>(frequency) / sampleRate;
    }

  float sample(){

    //phaseIncrement = (freq* 65536) / sampleRate;

    float val =0.0 ;

    switch (type) {
            case 0:
                val = amplitude * (phase < 0.5 ? 1.0 : -1.0);
                break;
            case 1:
                val = amplitude * sin(2 * M_PI * phase);
                break;
            case 2:
                val = amplitude * (2.0 * phase - 1.0);
                break;
            case 3:
                val = amplitude * (phase < 0.5 ? 4.0 * phase - 1.0 : 3.0 - 4.0 * phase);
                break;
        }
       // Serial.print("val:");
       // Serial.println(val);
        phase += phaseIncrement;
        if (phase >= 1.0) phase -= 1.0;
    return val;

  }
  void setType(int w){
    type = w;
  }
  void setAmpl(float ampl){
    amplitude = ampl;
  }

};




#endif 