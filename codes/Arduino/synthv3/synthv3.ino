#define CLK 1
#define DIN 2
#define LOAD 3

#define KEY1 33
#define KEY2 34
#define KEY3 35
#define KEY4 36
#define KEY5 37

#include "waves.h"

unsigned short val;
bool valueBits[12];

float currentT;

bool keysDown[25] = {0};
float freqSteps[25] = {0};
float currentSteps[25] = {0};
float timeDown[25] = {0};

void setup() {
  Serial.begin(9600);
  pinMode(DIN, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LOAD, OUTPUT);

  pinMode(KEY1, INPUT_PULLDOWN);
  pinMode(KEY2, INPUT_PULLDOWN);
  pinMode(KEY3, INPUT_PULLDOWN);
  pinMode(KEY4, INPUT_PULLDOWN);
  pinMode(KEY5, INPUT_PULLDOWN);

  // freqSteps[0] = 17.89;
  // freqSteps[1] = 30.1;
  // freqSteps[2] = 60.2;
  // freqSteps[3] = 110;

  freqSteps[0] = 138.6/246.3 * 2;
  freqSteps[1] = 130.8/246.3 * 2;
  freqSteps[2] = 123.5/246.3 * 2;
  freqSteps[3] = 110.0/246.3 * 2;
  freqSteps[4] = 103.8/246.3 * 2;


  val = 1;
  currentT = 0;
}

void SetDAC(unsigned short value) {

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

void updateKeysDown() {

  keysDown[0] = digitalRead(KEY1);
  keysDown[1] = digitalRead(KEY2);
  keysDown[2] = digitalRead(KEY3);
  keysDown[3] = digitalRead(KEY4);
  keysDown[4] = digitalRead(KEY5);
}

int testSum = 0;

void loop() {

  updateKeysDown();

  float sum = 0;
  float numDown = 0;

  for(int i=0;i<25;i++) {
    if(keysDown[i]) {
      timeDown[i]++;
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




