
/*
 Stepper Motor Control - one step at a time

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor will step one step at a time, very slowly.  You can use this to
 test that you've got the four wires of your stepper wired to the correct
 pins. If wired correctly, all steps should be in the same direction.

 Use this also to count the number of steps per revolution of your motor,
 if you don't know it.  Then plug that number into the oneRevolution
 example to see if you got it right.

 Created 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper stepper2(stepsPerRevolution, 2, 3, 4, 5);
Stepper stepper1(stepsPerRevolution, 8, 9, 10, 11);

int stepCount1 = 0;         // number of steps the motor has taken
int stepCount2 = 0;         // number of steps the motor has taken
void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

//void loop() {
  // step one step:
  //stepper2.step(1);
  //Serial.print("stepper 2 :");
  //Serial.println(stepCount2);
  //stepCount2++;
  //delay(3000);

    //stepper1.step(1);
  //Serial.print("steper 1:");
  //Serial.println(stepCount1);
 // stepCount1++;
  //delay(3000);
//}

void loop() {

int wavelength = 30;  // set to ten, will become keypad input
int ratio = 1;  //#ratio of steps/wavelength
int s = int(wavelength*ratio);              //#steps to obtain wavelength
Serial.write(wavelength);
Serial.write(s);
 
int j=1;
//while (j <= s) ;             #step out to wavelength
for (int j = 0; j <= s; j++){
 stepper2.step(1);
  Serial.print("stepper 2 :");
  Serial.println(stepCount2);
  stepCount2++;
  delay(300);
    Serial.write(j);
}
setlow();
//raw_input("Press enter to continue:");  //pauses program and stepper until enter is hit

int k=1;
for (int k=0; k<=s; k++){            // #return to original position
      stepper1.step(1);
  Serial.print("stepper 1:");
  Serial.println(stepCount1);
  stepCount1++;
  delay(300);
    Serial.write(k);
}
}


int setlow(){                         // #sets all pins to low to avoid direction issues from leaving a pin high
    digitalWrite(23,0);
    digitalWrite(24,0);
    digitalWrite(16,0);
    digitalWrite(20,0);
    digitalWrite(5,0);
    digitalWrite(3,0);
    digitalWrite(18,0);
    digitalWrite(2,0);
    return;
}
