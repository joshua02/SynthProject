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
#include <elapsedMillis.h>



bool keysDown[25] = {0}; //for keeping track of which keys are down
float freqSteps[25] = {0}; //how large of a step to take every cycle of loop for each key
float currentSteps[25] = {0}; //where the key is at in its cycle

elapsedMicros pressTime[25]; //how long since each key has been pressed
elapsedMicros releaseTime[25]; //how long since each key has been released

int waveMode = 0; //which waveform to use, defined in waves.h
bool menuButtonPressed = false;

bool valueBits[12]; //for the SetDAC function

float sum;  //for the doSample function
float numDown;

Encoder menuEncoder(ENCODER1, ENCODER2);
int encoderStep = 0;
int newStep;

LiquidCrystal menuLCD(LCD_RS, LCD_RW, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  Serial.begin(9600); //is this needed?
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LOAD, OUTPUT);

  //keyPins defined in pins.h
  for(int i=0;i<25;i++) { //set pinmode for keys
    pinMode(keyPins[i], INPUT_PULLDOWN);
  }

  pinMode(MENUBUTTON, INPUT_PULLDOWN);
  //TODO: pinmode for encoder?
  pinMode(ENCODER1, INPUT_PULLDOWN);
  pinMode(ENCODER2, INPUT_PULLDOWN);

  freqSteps[0] = 138.6/246.3 * 2; //TODO: store frequency data in a different table and change them depending on user input
  freqSteps[1] = 130.8/246.3 ;
  freqSteps[2] = 123.5/246.3 ;
  freqSteps[3] = 110.0/246.3 ;
  freqSteps[4] = 103.8/246.3 ;

  attachInterrupt(digitalPinToInterrupt(KEY1), key1ISR, CHANGE); //TODO: attach interrupts for all keys
  attachInterrupt(digitalPinToInterrupt(KEY2), key2ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY3), key3ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY4), key4ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY5), key5ISR, CHANGE);

  attachInterrupt(digitalPinToInterrupt(MENUBUTTON), menuButtonISR, RISING);

  menuLCD.begin(16, 2);
  // Print a message to the LCD.
  menuLCD.setCursor(0, 0);
  menuLCD.print("synth :)");

  keysDown[0] = 1; //for testing
  waveMode = 3;
}


bool newKeyRead;
//generic function all key ISR will call
void updateKey(int key, int pinNumber) {

  newKeyRead = digitalRead(pinNumber);
  if(keysDown[key-1] == 1 && newKeyRead == 0) {
    //key released
    releaseTime[key-1] = micros();
  }
  if (keysDown[key-1] == 0 && newKeyRead == 1) {
    //key pressed
    pressTime[key-1] = micros();
  }

  keysDown[key-1] = newKeyRead;
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
  sum = sum / numDown;
  setDAC((int) 2047 * sum + 2048);
}


float maxAmp = 1.0;
float sustainFactor = 0.6;

int attackTime = 10; //number of milliseconds per phase
int decayTime = 10;
int releaseTime = 10;

float adsrMillis;
//linear adsr envelope
void getADSRFactor(int key, bool pressed) {
  if(pressed) {
    adsrMillis = pressTime[key] / 1000.0f;
    if(adsrMillis < attackTime) {
      //attack phase
      return maxAmp * adsrMillis / attackTime; //linear between 0 (just pressed) and 1 (end of attack phase)
    } else if (adsrMillis < attackTime + decayTime) {
      //decay phase
      //linear from maxAmp to the sustain amplitude
      return (maxAmp - (adsrMillis - attackTime) / decayTime) * (1 - sustainFactor) + sustainFactor;
    } else {
      //sustain phase
      return sustainFactor;
    }
  } else {
    //not pressed
    adsrMillis = releaseTime[key] / 1000.0f;
    if(adsrMillis < releaseTime) {
      //release phase
      return sustainFactor - (adsrMillis / releaseTime) * sustainFactor
    }
  }
  return 0;
}

void loop() {
  //TODO: determine how long a loop takes and keep track of how long keys are pressed in ms

  //reading encoder changes
  newStep = menuEncoder.read();
  if(newStep != encoderStep) {

    if(newStep < encoderStep) {
      //moved left
      menuLCD.setCursor(0,1);
      menuLCD.print("to the left");
    } else {
      //moved right
      menuLCD.setCursor(0,1);
      menuLCD.print("to the right");
    }

    encoderStep = newStep;
  }
  if(menuButtonPressed) {
    menuButtonPressed = false;
    //menu button was pressed

  }
  

  updateChannels();
  doSample();

  //menuLCD.setCursor(0, 1);
  // print the number of seconds since reset:
  //menuLCD.print(millis()/1000);
  //menuLCD.print(encoderStep);
  //Serial.println(encoderStep);
}




