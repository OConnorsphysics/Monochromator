
/*
This program is a draft of a final control program for the Coburn lab Monochromator. 
Developed to test the stepper motor and OLED screen communications
Liam O'Connor
 */

#include <Stepper.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <stdlib.h>   //library which includes ascii to in converter
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include "SparkFun_Qwiic_Keypad_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_keypad

KEYPAD keypad1; //Create instance of this object

//////////////////////////
// MicroOLED Definition //
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1 

//////////////////////////////////
// MicroOLED Object Declaration //
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
int wavelength;    //global variable for the wavelength the user enters
const int stepsPerWave = 21;           //conversion factor  of motor steps to wavelength change
int steps;                            //number of steps motor will move, equal to wavelength by stepsPerWave conversion factor
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for your motor
const int homeWavelength = 61;       //wavelength of home position
// initialize the stepper library on pins 2 through 5:
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5);

void setup() {                ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // initialize the serial port:
  Serial.begin(9600);               //connect to serial monitor 
  
  myStepper.setSpeed(50);                  // set the speed at 60 rpm:
   delay(100);
  pinMode(7, INPUT_PULLUP);                 // initalize home limit switch, Normally open switch returns 1/HIGH until pressed
  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
  
  randomSeed(analogRead(A0) + analogRead(A1));

}



void loop() {       //main loop   ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  Serial.println("Monochromator control");

  if (keypad1.begin() == false)   // Note, using begin() like this will use default I2C address, 0x4B. 
                  // You can pass begin() a different address like so: keypad1.begin(Wire, 0x4A).
  {
    Serial.println("Keypad does not appear to be connected. Please check wiring. Freezing...");
    while (1);
  }
  //Serial.print("Initialized. Firmware Version: ");
  //Serial.println(keypad1.getVersion());
  Serial.println("Enter a wavelength in nanometers between 1-9999 and press # to go.");
  texttoScreen();  // the  function to display the wavelength on the OLED
  getbutton();      //function to get keypad inputs
  gotoWavelength();
  Serial.println("end of program");
  delay(2000);       

}

void goHome(){                   //function to step one at a time until home switch is triggered, then roll  back 10 steps///////////////////////////
  Serial.println("Hit the * key to go home and reset the machine.");
   oled.setFontType(0);  // Set font to type 0,1, or 2 for size     
  oled.clear(PAGE);     // Clear the page
  oled.setCursor(0, 0); // Set cursor to top-left               //print to oled
  oled.print("wavelength =");
  oled.print(wavelength);
  oled.print(" hit * to go home");
  oled.display();       // Refresh the display 
  delay(1000);
  char button = keypad1.getButton();
  do{ keypad1.updateFIFO();         //do loop to wait until user hits **** key to send it home
       button = keypad1.getButton();
       Serial.print("button = ");
       Serial.println(button);       //testing printing variables
       delay(200);
       }while(button != '*');    
  do{                               // step reverse one step at a time until the home microswitch is triggered
    myStepper.step(-1);
}while(digitalRead(7) != 0);
delay(1000);
myStepper.step(-20);

Serial.print("I'm Home!");
 oled.setFontType(0);  // Set font to type 0,1, or 2 for size     
  oled.clear(PAGE);     // Clear the page
  oled.setCursor(0, 0); // Set cursor to top-left               //print to oled
  oled.print(" I'm home now!");
  oled.display();       // Refresh the display 
  delay(2000);

  
}

void gotoWavelength(){        //function to go number of steps until wavelength is reached/////////////////////////////////////////////////
  Serial.print(" wavelength in go to wavelength loop = ");
  Serial.println(wavelength);
  if (wavelength <= 990){                             //check that wavelength is within monochromator range
    steps = (wavelength + homeWavelength)*stepsPerWave;
    myStepper.step(steps);
    delay(100);
    
  goHome();
  }
  else{     
    printTitle("Out of range", 0);
  }
}

void getbutton(){         //function to get button inputs on keypad     /////////////////////////////////////////////////////////////////////////////////////
  Serial.println("get button function");
  
  keypad1.updateFIFO();  // necessary for keypad to pull button from stack to readable register
  int digits [5]= {1,1,1,1,1};          //5 coloumn array to store digits from fifo
  Serial.print(digits[0]);                  //not needed jut for debugging
  Serial.print(digits[1]);
  Serial.print(digits[2]);
  Serial.print(digits[3]);
  Serial.println(digits[4]);
  int i=0;                    //counter variable to controlarray index
  char button = keypad1.getButton();
  delay(50);
  Serial.print("button = ");
  Serial.println(button);       //testing printing variables
  if (button == -1)
  {
    Serial.println("No keypad detected");
    delay(1000);
  }
   
  else {
    Serial.println("in else loop");
   do{ keypad1.updateFIFO();  // don't need as it clears fifo each loop, left while debugging
       button = keypad1.getButton();
       Serial.print("button = ");
       Serial.println(button);       //testing printing variables
       Serial.println("in do while loop");
       delay(200);
       if (button == '*'){                                    // going home function here that back steps untill switch is hit then roll off and reset
         goHome();
         Serial.print(" Going Home");       
       }
       else if(button != 0)        // '1'||'2'||'3'||'4'||'5'||'6'||'7'||'8'||'9'|| '#'|| '*')
        {digits[i] = (button - '0');    //fill array with button presses allowing wavelength of 9999 to be entered
        i++;}                     //increment array position
     }while(button != '#');    // fill array until user hits pound key to go to enetered wavelength

 // if (digits[0] == '#'){          //series of if statments that take digits from array and create wavelength between 0-9999nm
 // }
  if (digits[1] == -13){          // check for -13 as it is ascii for #, which we are using as our enter/go key
    wavelength = digits[0];
  }
  else if (digits[2] == -13){
    wavelength = digits[0]*10 + digits[1];
  }
  else if (digits[3] == -13){
    wavelength = digits[0]*100 + digits[1]*10 + digits[2];
  }
  else if (digits[4] == -13){
    wavelength = digits[0]*1000 + digits[1]*100 + digits[2]*10 +digits[3];
  }
Serial.print("wavelength = ");          //print the wavelength value as the program sees it (if incorrect the  above array and if statements are buggy)
Serial.println(wavelength);
Serial.print(digits[0]);
Serial.print(digits[1]);
Serial.print(digits[2]);
Serial.print(digits[3]);
Serial.print(digits[4]);
delay(500);


// We prompt for G - but never check for it       ///block of code to wait for serial input to assist with debuging
//Serial.println("Press G and Enter to continue");
//  while(Serial.available() < 1)
  //{
  //}
// now we clear the serial buffer.
//while(Serial.available() > 0)
  //{
//byte dummyread = Serial.read();
  //}
                                                //////////////////testing code ends here
}}

// Center and print a small title///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font){
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;
  
  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  //oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
           //      middleY - (oled.getFontHeight() / 2));
  oled.setCursor(0, 0);     //set cursor  top left
  // Print the title:
  oled.print(title);
  oled.display();
  delay(3000);
  oled.clear(PAGE);
}


void texttoScreen()     //function to print to screen//////////////////////////////////////////////////////////////////////////////////////////
{
printTitle("Enter wavelength, hit # to go", 0);

  // Demonstrate font 1. 8x16. Let's use the print function
  // to display every character defined in this font.
  oled.setFontType(0);  // Set font to type 0,1, or 2 for size
  oled.clear(PAGE);     // Clear the page
  oled.setCursor(0, 0); // Set cursor to top-left
  // Print can be used to print a string to the screen:
  oled.print("wavelength =");
  oled.display();       // Refresh the display
  delay(2000);          // Delay a second and repeat
  oled.setFontType(1);  // Set font to type 0,1, or 2 for size
  oled.print(wavelength);
  oled.display();
  delay(2000);
  }
