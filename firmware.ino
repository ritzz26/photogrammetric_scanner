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

#include "multiCameraIrControl.h"
#include <Servo.h>

const int stepsPerRevolution = 200 * 64; // change this to fit the number of steps per revolution for your motor

Stepper myStepper(stepsPerRevolution, 20, 21); // initialize stepper
// Enable pin is 19.  Step pin is 20.  Direction pin is 21
Servo cameraServo;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pin assignments for SainSmart LCD Keypad Shield

DFR_Key keypad; // initialize keypad

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
const int numStepUp = 10;
const int numRotation = 10;
const int cameraOrigAngle = 0;

Nikon Camera(53); // change Nikon to any other supported brand

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Shapespeare Scanner");
    delay(2500);
    lcd.clear();

    // set the speed at 60 rpm:
    myStepper.setSpeed(2);

    pinMode(19, OUTPUT); // enable pin
    digitalWrite(19, HIGH);
}

void loop()
{

    //
    lcd.setCursor(0, 0);
    lcd.print("Steps:");
    lcd.setCursor(6, 0);
    lcd.print("   ");
    lcd.setCursor(6, 0);
    lcd.print(stepChoices[stepIndex]);
    lcd.setCursor(0, 1);
    if (runFlag == 1)
    {
        lcd.print("run    ");
    }
    else
    {
        lcd.print("stop    ");
    }

    localKey = keypad.getKey(); // read keypad

    if (localKey != SAMPLE_WAIT)
    {

        if (localKey == 0) // key has been released
        {
            keyChange = 1; // the next key data represents a new key press
        }

        if (localKey == 1 && keyChange == 1) // select=start sequence
        {
            if (runFlag == 0 && keyChange == 1)
            {
                runFlag = 1;
                waitFlag = 0;
                keyChange = 0;
            }
            if (runFlag == 1 && keyChange == 1)
            {
                runFlag = 0;
                // stepCount = 1;  //uncomment to reset stepCount every time the auto scan is stopped.
                keyChange = 0;
            }
        }

        if (localKey == 2 && keyChange == 1) // left=manual mode
        {
            keyChange = 0;
            lcd.setCursor(0, 1);
            lcd.print("manual");
            Camera.shutterNow();                                         // trigger exposure
            delay(postWait);                                             // wait for exposure to complete
            digitalWrite(19, LOW);                                       // activate stepper driver
            myStepper.step(stepsPerRevolution / stepChoices[stepIndex]); // advance stepper
            digitalWrite(19, HIGH);                                      // deactivate stepper driver to save power, heat and noise
            stepCount++;
        }

        if (localKey == 3 && keyChange == 1 && runFlag == 0) // up cycle through angle choices
        {
            keyChange = 0;
            if (stepIndex < numChoices - 1)
            {
                stepIndex++;
            }
            else
            {
                stepIndex = 0;
            }
        }

        if (localKey == 4 && keyChange == 1) // down
        {
            keyChange = 0;
            lcd.setCursor(0, 1);
            lcd.print("CCW step");
            digitalWrite(19, LOW);
            myStepper.step(-stepsPerRevolution / stepChoices[stepIndex]);
            digitalWrite(19, HIGH);
        }

        if (localKey == 5 && keyChange == 1) // right
        {
            keyChange = 0;
            lcd.setCursor(0, 1);
            lcd.print("CW step");
            digitalWrite(19, LOW);
            myStepper.step(stepsPerRevolution / stepChoices[stepIndex]);
            digitalWrite(19, HIGH);
        }
    }

    if (runFlag == 1) // sequence is running
    {

        if (stepCount > stepChoices[stepIndex]) // the revolution is complete.
        {
            runFlag = 0; // stop sequence
            lcd.setCursor(9, 1);
            lcd.print("Done   ");
        }

        // This interrupt based time delay allows us to still receive keypad input during the delay

        currentTime = millis();

        if (waitFlag == 0) // advance stepper and start wait timer
        {
            startWait = millis();
            waitFlag = 1; // start preshutter wait
            lcd.setCursor(9, 1);
            lcd.print("Exp#:   ");
            lcd.setCursor(13, 1);
            lcd.print(stepCount);

            digitalWrite(19, LOW);                                       // activate stepper driver
            myStepper.step(stepsPerRevolution / stepChoices[stepIndex]); // advance stepper
            digitalWrite(19, HIGH);                                      // deactivate stepper driver
        }

        if (waitFlag == 1) // when preshutter wait expires trigger shutter
        {

            if (currentTime - startWait >= preWait) // wait time has expired
            {

                Camera.shutterNow();  // trigger shutter
                startWait = millis(); // restart wait timer
                waitFlag = 2;         // initiate post shutter wait
                stepCount++;
            }

            if (waitFlag == 2) // wait after triggering shutter before moving motor
            {

                if (currentTime - startWait >= postWait) // wait time has expired
                {
                    waitFlag = 0; // done waiting
                }
            }
        }
    }
}

void move_camera_to_bottom()
{
    // TODO: move camera to bottom
    // set camera to original angle
    cameraServo.write(cameraOrigAngle);
}

void rotate_plate_once(){};

void shutter(){};

void move_camera_up(){};

void tilt_camera(int angle)
{
    cameraServo.write(angle);
    delay(400);
}

void auto_scan()
{
    move_camera_to_bottom();
    for (int i = 0; i < numStepUp; i++)
    {
        for (int j = 0; j < numRotation; j++)
        {
            rotate_plate_once();
            delay(100);
            shutter();
        }
        move_camera_up();
        tilt_camera(tiltAngles[j]);
    }
}
