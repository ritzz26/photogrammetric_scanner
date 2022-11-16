/*
Billi Laboratory
University of California, Los Angeles
Department of Orthopedic Surgery
Author: Iman H. Enayati

Controls
SELECT: Start/Stop automatic scan.
LEFT: Trigger exposure and advance one unit clockwise.
RIGHT: Step one unit clockwise without triggering camera.
DOWN: Step one unit counter clockwise without triggering camera.
UP: Cycle through options for number of exposures per revolution.
*/

//#include "multiCameraIrControl.h"
#include <Servo.h>

#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6

const int stepsPerRevolution = 200 * 64; // change this to fit the number of steps per revolution for your motor

//Stepper myStepper(stepsPerRevolution, 20, 21); // initialize stepper
// Enable pin is 19.  Step pin is 20.  Direction pin is 21
//Servo cameraServo;

//LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pin assignments for SainSmart LCD Keypad Shield

//DFR_Key keypad; // initialize keypad

int localKey = 0;
int lastKey = 0;
int keyChange = 1;                          // set true when key is released.  Prevents double triggers.
int stepChoices[] = {25, 50, 75, 100, 200}; // different numbers of exposures per revolution which may be selected.  Higher number = smaller angle change
int stepIndex = 0;                          // count exposures starting at 1
int numChoices = 5;                         // number of step choices
int runFlag = 0;                            // is scanner running
int stepCount = 1;                          // count exposures starting at 1
unsigned long startWait;                    // time we start the wait timer
unsigned long currentTime;                  // current time
const long preWait = 2000;                  // pre exposure pause in milis.  Allows the specimen to settle before exposure.
const long postWait = 2000;                 // post exposure pause in milis.  Allows time for the exposure to finish before moving.
int waitFlag = 0;                           // 0=ready to move 1=pre-exposure wait 2=post-exposure wait
const int tiltAngles[] = {10, 20, 30, 40, 50};
const int numStepUp = 1;
const int numRotation = 10;
const int cameraOrigAngle = 0;

//Nikon Camera(53); // change Nikon to any other supported brand

void setup()
{
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  resetEDPins(); //Set step, direction, microstep and enable pins to default states
}

void loop()
{
  digitalWrite(EN, LOW);
  auto_scan();
  exit(1);
}

int Step_Count = 0;
int Stepper1_rev = 200;

void move_camera_to_bottom()
{
    //cameraServo.write(cameraOrigAngle);
    delay(100);
    digitalWrite(dir, HIGH);
    for(int x= 0; x<(Stepper1_rev*4*Step_Count); x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  digitalWrite(dir, LOW);
}

void rotate_plate_once(){};

void shutter(){};



void move_camera_up(){
  
  digitalWrite(dir, LOW);
  for(int x= 0; x<(Stepper1_rev*4); x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
    //Iman likes 50 for delay
  }
  Step_Count++;
};

void tilt_camera(int angle)
{
    //cameraServo.write(angle);
    delay(400);
}

void auto_scan()
{
    for (int i = 0; i < numStepUp; i++)
    {
        /*for (int j = 0; j < numRotation; j++)
        {
            rotate_plate_once();
            delay(100);
            shutter();
        }*/
        move_camera_up();
        //tilt_camera(tiltAngles[j]);
    }
    move_camera_to_bottom();
}

void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
}
