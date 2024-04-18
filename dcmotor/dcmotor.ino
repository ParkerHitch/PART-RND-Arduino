#include <Servo.h>
//Servo motor;
int dirPin = 4;
int pwmPin = 5;
int brakePin = 6;
bool dirState;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(brakePin, OUTPUT);
  dirState = true;
}
void loop() {
  digitalWrite(dirPin, HIGH);
  analogWrite(pwmPin, 150);
  delay(1000);
  analogWrite(pwmPin, 0);
  delay(1000);
}