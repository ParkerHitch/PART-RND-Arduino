#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <math.h>

#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE

int dirPin = 4;
int pwmPin = 5;
bool dirState;

int destination = 0; 
volatile int posi = 0; // specify posi as volatile

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  pinMode(dirPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
}

void loop() {



  int pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }

  float err = destination - pos; //defines error
  motorPower(err * 1/16);
  
  Serial.println(pos);
}

void readEncoder(){
  int b = digitalRead(ENCB);
  if(b > 0){
    posi++;
  }
  else{
    posi--;
  }
}

void motorPower(float power) {
  digitalWrite(dirPin, power >= 0 ? HIGH : LOW);
  analogWrite(pwmPin, 255 * fabs(power));
}