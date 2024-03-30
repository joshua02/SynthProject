#define DIN 13
#define CLK 12
#define LOAD 8

//digital speed test

void setup() {
  pinMode(CLK, OUTPUT);
}

void loop() {
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
}

//using analogwrite for pwm test

// #define ANACLK 5

// void setup() {
//   pinMode(ANACLK, OUTPUT);
//   analogWrite(ANACLK, 128);
// }

// void loop() {

// }
