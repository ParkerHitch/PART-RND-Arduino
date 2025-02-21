#include <Servo.h>
#include <stdio.h>
#include "HX711.h"

#define LOADCELL_TICKS_PER_KG 102842
#define LOOP_DELAY_MS 10

#define ESC_PIN 9
#define ESC_PWM_MIN 1100
#define ESC_PWM_MAX 1900

#define SERVO_PIN 10
#define SERVO_PWM_HORIZ 1815
#define SERVO_PWM_VERT 1155

#define SERVO_PWM_MAX (SERVO_PWM_VERT > SERVO_PWM_HORIZ ? SERVO_PWM_VERT : SERVO_PWM_HORIZ)
#define SERVO_PWM_MIN (SERVO_PWM_VERT > SERVO_PWM_HORIZ ? SERVO_PWM_HORIZ : SERVO_PWM_VERT)

// Measured in degrees per second
#define SERVO_SLEW_RATE_DEG_SEC 45
#define SERVO_SLEW_RATE_US_SEC (SERVO_SLEW_RATE_DEG_SEC * (SERVO_PWM_MAX-SERVO_PWM_MIN) / 90)

#define LOAD_SCK_PIN 4
#define LOAD_DT_PIN 5

Servo esc;  //esc
Servo servo;
HX711 load;

int currentEscPercent = 0;
int readingCounter = 0;
double reading;

long currentServoUs = 0;
long targetServoUs = 0;
unsigned long startedSlewingTime = 0;
int startedSlewingUs = 0;
bool doneSlewing = true;

void setup() {
  Serial.begin(9600);

  servo.attach(SERVO_PIN, SERVO_PWM_MIN, SERVO_PWM_MAX);
  esc.attach(ESC_PIN, ESC_PWM_MIN, ESC_PWM_MAX);

  load.begin(LOAD_DT_PIN, LOAD_SCK_PIN, 0);

  esc.writeMicroseconds(ESC_PWM_MIN);
  servo.writeMicroseconds(SERVO_PWM_VERT);
  currentServoUs=targetServoUs=SERVO_PWM_VERT;
}

void loop() {
  if (Serial.available()) {
    int nextChar = Serial.read();
    // Percent command:
    //   Of form "Pxxx", where xxx = percent to go to
    if (nextChar == 'P') {
      currentEscPercent = Serial.parseInt();
      if (currentEscPercent <= 100 && currentEscPercent >= 0) {
        writePwmPercent(currentEscPercent);
      }
      // Servo command, one of forms:
      //    "Sxx", where xx = degrees. 0 = down, 90 = up
      //    "SRxx", where xx = raw value for servo (in us). Should only be used for debugging
    } else if (nextChar == 'S') {
      long newServoUs;

      if (Serial.peek() == 'R') {
        Serial.read();
        newServoUs = Serial.parseInt();
        Serial.print("New us: ");
        Serial.println(newServoUs);
        servo.writeMicroseconds(newServoUs);
        currentServoUs = newServoUs;
        doneSlewing = true;
      } else {
        long newAng = Serial.parseInt();
        Serial.print("target angle: ");
        Serial.println(newAng);
        newServoUs = ((SERVO_PWM_VERT - SERVO_PWM_HORIZ) * newAng / 90) + SERVO_PWM_HORIZ;  
        startedSlewingUs = currentServoUs;
        startedSlewingTime = millis();
        doneSlewing = false;
      }

      targetServoUs = newServoUs;

      // Tare command, of form:
      //    "TARE", tares the load cell
    } else if (nextChar == 'T') {
      char buff[3] = {};
      size_t read = Serial.readBytes(buff, 3);
      if (read == 3 && memcmp(read, "ARE", 3)) {
        // Tare time:
        load.tare();
      }
    } else if (nextChar != '\n') {
      currentEscPercent = 0;
      writePwmPercent(currentEscPercent);
    }
  }

  if (doneSlewing) {
    servo.writeMicroseconds(targetServoUs);
  } else {
    if (targetServoUs > currentServoUs) {
      currentServoUs = startedSlewingUs + (millis() - startedSlewingTime) * SERVO_SLEW_RATE_US_SEC / 1000;
      if (currentServoUs > targetServoUs) {
        doneSlewing = true;
        Serial.println("Done slewing!");
        currentServoUs = targetServoUs;
      }
    } else {
      currentServoUs = startedSlewingUs - (millis() - startedSlewingTime) * SERVO_SLEW_RATE_US_SEC / 1000;
      // Target < current
      if (currentServoUs < targetServoUs) {
        doneSlewing = true;
        Serial.println("Done slewing!");
        currentServoUs = targetServoUs;
      }
    }
    servo.writeMicroseconds(currentServoUs);
  }
  

  readingCounter++;
  if (readingCounter == 10) {
    readingCounter = 0;
    if (load.is_ready()) {
      load.set_scale(1);
      reading = load.get_units(1);
    }

    Serial.print(currentEscPercent);
    Serial.print(", ");
    Serial.print(reading / LOADCELL_TICKS_PER_KG);
    Serial.print(", ");
    Serial.println(90*(currentServoUs-SERVO_PWM_HORIZ)/(SERVO_PWM_VERT-SERVO_PWM_HORIZ));
  }

  delay(LOOP_DELAY_MS);
}

void writePwmPercent(int percent) {
  int new_us = ((ESC_PWM_MAX - ESC_PWM_MIN) / 100 * percent) + ESC_PWM_MIN;
  esc.writeMicroseconds(new_us);
  Serial.print("PWM set to ");
  Serial.print(new_us);
  Serial.println("us");
}