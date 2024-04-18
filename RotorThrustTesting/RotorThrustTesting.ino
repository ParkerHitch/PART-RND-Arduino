#include <Servo.h>
#include "HX711.h"

#define IN_BUFF_SZ 20

#define CALIB_READING 219252
#define CALIB_KNOWN 5

#define ESC_PWM_PIN 3

#define LOAD_SCK_PIN 4
#define LOAD_DT_PIN 5 

Servo esc;
HX711 load;

int state;
char inBuff[IN_BUFF_SZ];
int inBuffInd = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esc.attach(ESC_PWM_PIN, 1000, 2000);
  load.begin(LOAD_DT_PIN, LOAD_SCK_PIN, 0);

  state = 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  if(state == 0){
    esc.writeMicroseconds(0);
    Serial.print("Enter New State: ");
    while(inBuffInd < IN_BUFF_SZ){
      if(Serial.available()){
        inBuff[inBuffInd] = Serial.read();
        if(inBuff[inBuffInd] == '\n'){
          inBuff[inBuffInd] = 0;
          break;
        }
        inBuffInd++;
      }
    }
    if(inBuffInd == IN_BUFF_SZ){
      serialFlush();
      Serial.print("\nError: Input too long!\n");
    } else {
      Serial.print("\nSwitching state to: ");
      Serial.print(inBuff);
      Serial.print('\n');
      state = atoi(inBuff); 
    }
    inBuffInd = 0;
  } else if(state==100) {
    if (load.is_ready()) {
      load.set_scale();    
      Serial.println("Tare... remove any weights from the load.");
      delay(5000);
      load.tare();
      Serial.println("Tare done...");
      Serial.print("Place a known weight on the load...");
      delay(5000);
      long reading = load.get_units(10);
      Serial.print("Result: ");
      Serial.println(reading);
      state = 0;
    }
  }  else if(state==101) {
    if (load.is_ready()) {
      Serial.println("Tare");
      load.tare();
      Serial.println("Tare done...");
      state = 0;
    }
  }  else if(state==102) {
    if (load.is_ready()) {
      load.set_scale(1);
      double reading = load.get_units(10);
      Serial.print("Result: ");
      Serial.println((reading / 96775));
      state = 0;
    }
  } else if(state==103) {
    if (load.is_ready()) {
      load.set_scale(1);
      double reading = load.get_units(1);
      Serial.println((reading / 96775));
    }
  } else {
    Serial.print("\nInvalid State: ");
    Serial.println(String(state));
    state = 0;
  }



}

void serialFlush(){
  while(Serial.available()) {
    char t = Serial.read();
  }
}
