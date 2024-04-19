// send pwm to esc, wait, take reading from load cell, print into savable format 
// takes pwm input for beginning and end values and it smoothly increases over 5s
// takes reading from load cell for each loop, and pwm into savable format
#include <Servo.h>
#include <stdio.h>
#include "HX711.h"

#define LOAD_SCK_PIN 4
#define LOAD_DT_PIN 5
#define ESC_PIN 3 

Servo servo; //esc
HX711 load;

int pwmMin = 1000; 
int pwmMax = 2000;
int pwmVal = pwmMin;
int increment = 10;

int running = true;

void setup() {
  Serial.begin(9600);
  
  //ESC STUFF
  servo.attach(ESC_PIN);
  servo.writeMicroseconds(1500); //idk what this does, "sends stop signal... necessary for arming"
  
  //LOAD CELL STUFF
  load.begin(LOAD_DT_PIN, LOAD_SCK_PIN, 0);
}

void loop() {
  if (running) {
  
    //LOAD CELL LOOP
    if (load.is_ready()) {
      load.set_scale(1);
      double reading = load.get_units(1);
      Serial.println((reading / 96775));
    }

    //ESC INCREMENT LOOP
    servo.writeMicroseconds(pwmVal); //sends signal to ESC
    Serial.println(pwmVal);
    pwmVal += increment;

    if (pwmVal >= pwmMax) {
      pwmVal = 1000;
      running = false;
    }

    delay(100); //takes 10s to get from 1000 to 2000 w/ +10 every 100ms
  }
}
