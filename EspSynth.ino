#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/dac.h"
#include <math.h>





#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I2C address for the OLED display (usually 0x3C for 128x64 OLEDs)
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

TaskHandle_t displayTaskHandle;


const int dacPin = 25;

const int potPin1=33;
const int potPin2=32;
const int potPin3=35;
const int modePin=13;

const int btP1 = 15;
const int btP2 = 2;
const int btP3 = 0;
const int btP4 = 4;
const int btP5 = 16;
const int btP6 = 17;
const int btP7 = 5;
const int btP8 = 18;
const int btP9 = 19;
const int btP10 = 27;
const int btP11 = 14;
const int btP12 = 12;


uint8_t notes[8][12] = {
    // Octave 0
    {16, 17, 18, 19, 21, 23, 25, 26, 28, 30, 32, 34},
    // Octave 1
    {33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62},
    // Octave 2
    {65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123},
    // Octave 3
    {131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247},
    // Octave 4
    {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494},
    // Octave 5
    {523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988},
    // Octave 6
    {1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976},
    // Octave 7
    {2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951}
};


int prevMPStatus= HIGH;
int mode = 0;
//const uint8_t audioSample[] = {128, 130, 132, 128, 126, 124, 128, 130}; // Example values
//int sampleLength = sizeof(audioSample) / sizeof(audioSample[0]);
//int sampleRate = 8000;



int bpm = 40;
int wav = 0;
int oct = 1;
int currentNote = notes[oct-1][0];

unsigned long previousMicros = 0;
int t = 0;


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

    int val;

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

Oscillator osc;

bool play = false;


void render(void *parameter){
  for (;;) {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.print("M: ");
  display.println(mode);
  display.print("bpm: ");
  display.println(bpm);
  display.print("oct: ");
  display.println(oct);
  if (wav == 0){
    display.println("wav: sqr");
  }
  else if (wav == 1){
    display.println("wav: sin");
  }
  else if (wav == 2){
    display.println("wav: saw");
  }
  else if (wav == 3){
    display.println("wav: tri");
  }
  display.print("Nt: ");
  display.println(currentNote);

  display.display();
  vTaskDelay(100 / portTICK_PERIOD_MS);}
}

void setup() {
  Serial.begin(115200);
  

  pinMode(potPin1,INPUT);
  pinMode(potPin2,INPUT);
  pinMode(potPin3,INPUT);
  pinMode(modePin,INPUT_PULLUP);

  pinMode(btP1,INPUT_PULLUP);
  pinMode(btP2,INPUT_PULLUP);
  pinMode(btP3,INPUT_PULLUP);
  pinMode(btP4,INPUT_PULLUP);
  pinMode(btP5,INPUT_PULLUP);
  pinMode(btP6,INPUT_PULLUP);
  pinMode(btP7,INPUT_PULLUP);
  pinMode(btP8,INPUT_PULLUP);
  pinMode(btP9,INPUT_PULLUP);
  pinMode(btP10,INPUT_PULLUP);
  pinMode(btP11,INPUT_PULLUP);
  pinMode(btP12,INPUT_PULLUP);

   xTaskCreatePinnedToCore(
        render,  // Task function
        "DisplayUpdate",    // Task name
        2048,               // Stack size (in bytes)
        NULL,               // Task parameter
        1,                  // Task priority
        &displayTaskHandle, // Task handle
        1                   // Core to pin to (Core 1)
    );



  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  display.clearDisplay();
 


}

void loop() {
  //Serial.println("correct");
  int modeBt = digitalRead(modePin);
  int bt1 = digitalRead(btP1);
  int bt2 = digitalRead(btP2);
  int bt3 = digitalRead(btP3);
  int bt4 = digitalRead(btP4);
  int bt5 = digitalRead(btP5);
  int bt6 = digitalRead(btP6);
  int bt7 = digitalRead(btP7);
  int bt8 = digitalRead(btP8);
  int bt9 = digitalRead(btP9);
  int bt10 = digitalRead(btP10);
  int bt11 = digitalRead(btP11);
  int bt12 = digitalRead(btP12);
  // Mode handeling
  //Serial.println(modeBt);
  if (modeBt == LOW && prevMPStatus == HIGH ){
    mode++;
    if (mode>4){
      mode = 0;
    }
  }
  prevMPStatus = modeBt;
  //setMode(mode);
  potFunc1();
  potFunc2();
  potFunc3();
  //Serial.println(bt1);
  play = false;
  if (bt1 == LOW){
    currentNote = notes[oct-1][0];
    play = true;
   
  }
  if (bt2 == LOW){
    currentNote= notes[oct-1][1];
    //Serial.println(2);
    play = true;
  }
  if (bt3 == LOW){
    currentNote= notes[oct-1][2];
    //Serial.println(3);
    play = true;
  }
  if (bt4 == LOW){
    currentNote= notes[oct-1][3];
    //Serial.println(4);
    play = true;
  }
  if (bt5 == LOW){
    currentNote= notes[oct-1][4];
    //Serial.println(5);
    play = true;
  }
  if (bt6 == LOW){
    currentNote= notes[oct-1][5];
    //Serial.println(6);
    play = true;
  }
  if (bt7 == LOW){
    currentNote= notes[oct-1][6];
    //Serial.println(7);
    play = true;
  }
  if (bt8 == LOW){
    currentNote= notes[oct-1][7];
    //Serial.println(8);
    play = true;
  }
  if (bt9 == LOW){
    currentNote= notes[oct-1][8];
    //Serial.println(9);
    play = true;
  }
  if (bt10 == LOW){
    currentNote= notes[oct-1][9];
    //Serial.println(10);
    play = true;
  }
  if (bt11 == LOW){
    currentNote= notes[oct-1][10];
    //Serial.println(11);
    play = true;
  }
  if (bt12 == LOW){
    currentNote= notes[oct-1][11];
    //Serial.println(12);
    play = true;
  }
  //playNote(0);
  audioPipeline(play);
  
  //render();
}


void potFunc1(){
  int i = analogRead(potPin1);
  //Serial.println(i);
  if (mode==0){
    bpm = map(i, 0, 4095, 40, 200);
  }
  
  
}
void potFunc2(){
  
  int j = analogRead(potPin2);
  //Serial.println(j);
  if (mode==0){oct = map(j, 0, 4095, 1, 8);}
  
 
}
void potFunc3(){
  
  int k = analogRead(potPin3);
  //Serial.println(k);
  if (mode==0){wav = map(k, 0, 4095, 0, 3);}
  osc.changeType(wav);
}


void playNote(int sample){
  dacWrite(dacPin,sample);
}





void audioPipeline(bool p){
  
  if (p){
    osc.setFrequency(currentNote);
    int sample = osc.sample();
    playNote(sample);
  }
  else{
    playNote(0);

  }
  

  
  delayMicroseconds(1000000 / osc.sampleRate);
}



