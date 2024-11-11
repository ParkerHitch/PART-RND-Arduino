#include <Servo.h>
#include <stdio.h>
#include "HX711.h"

#define IN_BUFF_SZ 20

#define CALIB_READING 219252
#define CALIB_KNOWN 5

#define ESC_PIN 9
#define SERVO_PIN 10

#define LOAD_SCK_PIN 4
#define LOAD_DT_PIN 5

Servo esc;  //esc
Servo servo;
HX711 load;

int pwmMin = 1100;
int pwmMax = 1900;
int status = 0;
int speedPercent = 0;
int counter = 0;
double reading;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN, pwmMin, pwmMax);
  esc.attach(ESC_PIN, pwmMin, pwmMax);
  load.begin(LOAD_DT_PIN, LOAD_SCK_PIN, 0);
  esc.writeMicroseconds(pwmMin);

  Serial.println("Enter starting speed: ");
}

void loop() {
  servo.write(52);
  if (Serial.available()) {
    int nextChar = Serial.read();
    if (nextChar == 'P') {
      status = Serial.parseInt();
      if (status <= 100 && status >= 0) {
        speedPercent = parkersFunction(status);
      }
    } else if (nextChar == 'T') {
      char buff[3] = {};
      size_t read = Serial.readBytes(buff, 3);
      if (read == 3 && memcmp(read, "ARE", 3)){
        // Tare time:
        load.tare();
      }
    } else if (nextChar != 10) {
      status = 0;
      speedPercent = parkersFunction(status);
    }
  }

  counter++;
  if (counter == 10){
    counter = 0;
    if (load.is_ready()) {
      load.set_scale(1);
      reading = load.get_units(1);
    }

    //Serial.print("Speed (%): ");
    Serial.print(status);
    Serial.print(", ");
    //Serial.print("Load: ");
    // For tilt rotor:
    Serial.println(reading / 102842);
    // For back:
    // Serial.println(reading / 96775);
  }

  delay(10);
}

int parkersFunction(int status) {
  speedPercent = ((pwmMax - pwmMin) / 100) * status + pwmMin;
  esc.writeMicroseconds(speedPercent);
  Serial.print("PWM set to ");
  Serial.print(speedPercent);
  Serial.println("%");

  return (speedPercent);
}