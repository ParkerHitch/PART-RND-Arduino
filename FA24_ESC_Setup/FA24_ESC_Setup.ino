#include <Servo.h>
#include <stdio.h>
#include "HX711.h"

#define IN_BUFF_SZ 20

#define CALIB_READING 219252
#define CALIB_KNOWN 5

#define ESC_PIN 8

#define LOAD_SCK_PIN 4
#define LOAD_DT_PIN 5

Servo servo;  //esc
HX711 load;

int pwmMin = 1100;
int pwmMax = 1900;
int status = 0;
int speedPercent = 0;
double reading;

void setup() {
  Serial.begin(9600);
  servo.attach(ESC_PIN, pwmMin, pwmMax);
  load.begin(LOAD_DT_PIN, LOAD_SCK_PIN, 0);
  servo.writeMicroseconds(pwmMin);

  Serial.println("Enter starting speed: ");
}

void loop() {
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
        load.tare()
      }
    } else if (nextChar != 10) {
      status = 0;
      speedPercent = parkersFunction(status);
    }
  }

  if (load.is_ready()) {
    load.set_scale(1);
    reading = load.get_units(1);
  }

  //Serial.print("Speed (%): ");
  Serial.print(status);
  Serial.print(", ");
  //Serial.print("Load: ");
  Serial.println(reading / 96775);
  delay(200);
}

int parkersFunction(int status) {
  speedPercent = ((pwmMax - pwmMin) / 100) * status + pwmMin;
  servo.writeMicroseconds(speedPercent);
  Serial.print("PWM set to ");
  Serial.print(speedPercent);
  Serial.println("%");

  return (speedPercent);
}