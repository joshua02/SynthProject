#define CLK 52
#define DIN 50
#define LOAD 48



#define KEY1 8
#define KEY2 9
#define KEY3 10
#define KEY4 11

#include "waves.h"

unsigned short val;
bool valueBits[12];

float currentT;

bool keysDown[25] = {0};
float freqSteps[25] = {0};
float currentSteps[25] = {0};

void setup() {
  Serial.begin(9600);
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LOAD, OUTPUT);

  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  pinMode(KEY3, INPUT);

  // freqSteps[0] = 17.89;
  // freqSteps[1] = 30.1;
  // freqSteps[2] = 60.2;
  // freqSteps[3] = 110;

  freqSteps[0] = 30;
  freqSteps[1] = 45;
  freqSteps[2] = 60;
  freqSteps[3] = 120;


  val = 1;
  currentT = 0;
}

void SetDAC(unsigned short value) {

  digitalWrite(DIN,HIGH);

  for(int i=0;i<12;i++){
    valueBits[12-i-1] = value % 2;
    value = value >> 1;
  }

  for(int i=0;i<12;i++){
      digitalWrite(CLK,LOW);
      digitalWrite(DIN, valueBits[i]);
      //digitalWrite(DIN, value % 2);
      digitalWrite(CLK,HIGH);
  }

  digitalWrite(LOAD,LOW);
  digitalWrite(LOAD,HIGH);
  
}

void updateKeysDown() {

  keysDown[0] = digitalRead(KEY1);
  keysDown[1] = digitalRead(KEY2);
  keysDown[2] = digitalRead(KEY3);
  keysDown[3] = digitalRead(KEY4);
}

void loop() {

  updateKeysDown();

  float sum = 0;
  float numDown = 0;

  for(int i=0;i<25;i++) {
    if(keysDown[i]) {
      numDown++;
      sum += sineWave[(int) currentSteps[i]];
      currentSteps[i] += freqSteps[i];
      if (currentSteps[i] > 360) {
        currentSteps[i] = (int) currentSteps[i] % 360;
      }
    }
  }

  sum = sum / numDown;

  SetDAC((int) 2047 * sum + 2048);
}




