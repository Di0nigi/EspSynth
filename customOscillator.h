#ifndef CUSTOMOSCILLATOR_H  // Prevents multiple inclusions
#define CUSTOMOSCILLATOR_H

// Add necessary includes
#include <Arduino.h>
#include <math.h>


class Oscillator{
  public: 
  int amplitude = 127;
  int sampleRate = 8000;
  unsigned long phase=0;  // Phase accumulator for waveform
  int phaseIncrement=0; 
  int frequency = 0;


  int type = 0;
  Oscillator(){};
  void setFrequency(int freq) {
        frequency = freq;
        phaseIncrement = (frequency * 65536) / sampleRate;
    }

  int sample(){

    //phaseIncrement = (freq* 65536) / sampleRate;

    int val=0;

    switch (type) {
            case 0: // Sine wave
                val = amplitude * sin(2 * PI * phase / 65536) + 128;
                break;
            case 1: // Square wave
                val = (phase < 32768) ? amplitude + 128 : 128 - amplitude;
                break;
            case 2: // Sawtooth wave
                val = (phase >> 8) - 128;
                break;
            case 3: // Triangle wave
                val = (phase < 32768) ? (phase >> 7) - 128 : 127 - (phase >> 7);
                break;
        }

        phase = (phase + phaseIncrement) & 0xFFFF;
    return val;

  }
  void changeType(int w){

    type = w;

  }

};




#endif 