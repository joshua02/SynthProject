/*
Microcontroller Code for the Synthesizer
Using the Teensy 4.1 Board

Written by Joshua Wilkinson for SIUE Senior Design Project
*/

#include "pins.h"
#include "waves.h"
#include "freq.h"
#include <LiquidCrystal.h>
#include <Encoder.h>

bool valueBits[12]; //for the SetDAC function

float sum;  //for the doSample function
float numDown;

bool keysDown[25] = {0}; //for keeping track of which keys are down
float freqSteps[25] = {0}; //how large of a step to take every cycle of loop for each key
float currentSteps[25] = {0}; //where the key is at in its cycle

int waveMode = 0; //which waveform to use, defined in waves.h
bool menuButtonPressed = false;

Encoder menuEncoder(ENCODER1, ENCODER2);
int encoderStep = -999;
int newStep;


void setup() {
  Serial.begin(9600); //is this needed?
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LOAD, OUTPUT);

  //keyPins defined in pins.h
  for(int i=0;i<25;i++) {
    pinMode(keyPins[i], INPUT_PULLDOWN);
  }
  pinMode(MENUBUTTON, INPUT_PULLDOWN);
  //TODO: pinmode for encoder?

  freqSteps[0] = 138.6/246.3 * 2; //TODO: store frequency data in a different table and change them depending on user input
  freqSteps[1] = 130.8/246.3 * 2;
  freqSteps[2] = 123.5/246.3 * 2;
  freqSteps[3] = 110.0/246.3 * 2;
  freqSteps[4] = 103.8/246.3 * 2;

  attachInterrupt(digitalPinToInterrupt(KEY1), key1ISR, CHANGE); //TODO: attach interrupts for all keys
  attachInterrupt(digitalPinToInterrupt(KEY2), key2ISR, CHANGE);

  attachInterrupt(digitalPinToInterrupt(MENUBUTTON), menuButtonISR, RISING);
}

//generic function all key ISR will call
void updateKey(int key, int pinNumber) {
  keysDown[key-1] = digitalRead(pinNumber);
}

void key1ISR() { updateKey(1, KEY1); }
void key2ISR() { updateKey(2, KEY2); }
void key3ISR() { updateKey(3, KEY3); }
void key4ISR() { updateKey(4, KEY4); }
void key5ISR() { updateKey(5, KEY5); }
void key6ISR() { updateKey(6, KEY6); }
void key7ISR() { updateKey(7, KEY7); }
void key8ISR() { updateKey(8, KEY8); }
void key9ISR() { updateKey(9, KEY9); }
void key10ISR() { updateKey(10, KEY10); }
void key11ISR() { updateKey(11, KEY11); }
void key12ISR() { updateKey(12, KEY12); }
void key13ISR() { updateKey(13, KEY13); }
void key14ISR() { updateKey(14, KEY14); }
void key15ISR() { updateKey(15, KEY15); }
void key16ISR() { updateKey(16, KEY16); }
void key17ISR() { updateKey(17, KEY17); }
void key18ISR() { updateKey(18, KEY18); }
void key19ISR() { updateKey(19, KEY19); }
void key20ISR() { updateKey(20, KEY20); }
void key21ISR() { updateKey(21, KEY21); }
void key22ISR() { updateKey(22, KEY22); }
void key23ISR() { updateKey(23, KEY23); }
void key24ISR() { updateKey(24, KEY24); }
void key25ISR() { updateKey(25, KEY25); }

void menuButtonISR() {
  menuButtonPressed = true;
}

//to drive the LTC1257 digital to analog converter
void setDAC(unsigned short value) {
  digitalWrite(CLK,HIGH);

  for(int i=0;i<12;i++){
    valueBits[12-i-1] = value % 2;
    value = value >> 1;
  }

  for(int i=0;i<12;i++){
      digitalWrite(CLK,LOW);
      delayNanoseconds(250); //t4 = 250 + 100
      digitalWrite(DIN, valueBits[i]);
      delayNanoseconds(100); // t1
      digitalWrite(CLK,HIGH);
      delayNanoseconds(350); //t3
  }

  digitalWrite(LOAD,LOW);
  delayNanoseconds(150); //t5
  digitalWrite(LOAD,HIGH);
  
}

//increment each key's channel if it's pressed down
void updateChannels() {
  for(int i=0;i<25;i++) {
    if(keysDown[i]) {
      currentSteps[i] += freqSteps[i];
      if(currentSteps[i] > 360) {
        currentSteps[i] = (int) currentSteps[i] % 360;
      }
    }
  }
}

//determine what the DAC voltage needs to be and sets it to produce that voltage
void doSample() {
  sum = 0;
  numDown = 0;
  switch(waveMode) {
    case SINE:
      for(int i=0;i<25;i++) {
        if(keysDown[i]) {
          numDown++;
          sum += sineWave[(int) currentSteps[i]];
        }
      }
      break;
    case SAWTOOTH:
      for(int i=0;i<25;i++) {
        if(keysDown[i]) {
          numDown++;
          sum += sawtoothWave[(int) currentSteps[i]];
        }
      }
      break;
    case SQUARE:
      for(int i=0;i<25;i++) {
        if(keysDown[i]) {
          numDown++;
          sum += squareWave[(int) currentSteps[i]];
        }
      }
      break;
    case TRIANGLE:
      for(int i=0;i<25;i++) {
        if(keysDown[i]) {
          numDown++;
          sum += triangleWave[(int) currentSteps[i]];
        }
      }
      break;
  }
  // if(waveMode == SINE) {
  //   for(int i=0;i<25;i++) {
  //     if(keysDown[i]) {
  //       numDown++;
  //       sum += sineWave[(int) currentSteps[i]];
  //     }
  //   }
  // } else if(waveMode == SAWTOOTH) {
  //   for(int i=0;i<25;i++) {
  //     if(keysDown[i]) {
  //       numDown++;
  //       sum += sawtoothWave[(int) currentSteps[i]];
  //     }
  //   }
  // } else if(waveMode == SQUARE) {
  //   for(int i=0;i<25;i++) {
  //     if(keysDown[i]) {
  //       numDown++;
  //       sum += squareWave[(int) currentSteps[i]];
  //     }
  //   }
  // } else if(waveMode == TRIANGLE) {
  //   for(int i=0;i<25;i++) {
  //     if(keysDown[i]) {
  //       numDown++;
  //       sum += triangleWave[(int) currentSteps[i]];
  //     }
  //   }
  // }
  sum = sum / numDown;
  setDAC((int) 2047 * sum + 2048);
}


void loop() {
  //TODO: determine how long a loop takes and keep track of how long keys are pressed in ms

  //reading encoder changes
  newStep = menuEncoder.read();
  if(newStep != encoderStep) {

    if(newStep > encoderStep) {
      //moved left

    } else {
      //moved right
      
    }

    encoderStep = newStep;
  }
  if(menuButtonPressed) {
    menuButtonPressed = false;
    //menu button was pressed

  }
  

  updateChannels();
  doSample();

}




