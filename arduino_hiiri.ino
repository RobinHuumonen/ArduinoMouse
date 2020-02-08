#include <RH_ASK.h>
#include <SPI.h>
#include <math.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Mouse.h>
#include <MsTimer2.h>
MPU6050 mpu;

float pitch,roll, pitch_out, roll_out ;
float mouseYFromRoll, mouseXFromPitch, temp;
int16_t ax, ay, az, gx, gy, gz;
float vx, vy, vx_out, vy_out;
float new_coefficient = 0.03;
float old_coefficient = 0.97;

const int mouse1 = 5;
const int  mouse2 = 6;
const int mouse3 = 7;
const int mouse4 = 8;

bool mouse1State = 0;
bool mouse2State = 0;
bool mouse3State = 0;
bool mouse4State = 0;
int mouse3PressCount = 0;

bool mouse1LastState = 0;
bool mouse2LastState = 0;
bool mouse3LastState = 0;
bool mouse4LastState = 0;

float sensitivityCAL = 0.0;
const char * buttonCMD; // 7 characters

void setup()
{
    //MsTimer2::set(1000000, getScroll);
    //MsTimer2::start();
    Serial.begin(115200);
    pinMode(mouse1, INPUT);
    pinMode(mouse2, INPUT);
    pinMode(mouse3, INPUT);
    pinMode(mouse4, INPUT);
    Wire.begin();
    mpu.initialize();
    if (!mpu.testConnection())
    {
        while (1);
    }
}


void loop()
{
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    getVyVx ();
    buttonStates();
    mouseCtrl();
}

void getVyVx ()
{
    vy = -(gx+200)/150;
    vx = -(gz-150)/150;
    vy_out = new_coefficient * vy + old_coefficient * vy_out;
    vx_out = new_coefficient * vx + old_coefficient * vx_out;

}

void mouseCtrl()
{
    float x, y;
    x = vx_out / sensitivityCAL;
    y = vy_out / sensitivityCAL;
    Mouse.move(x, y, 0);

    if (buttonCMD == "mouse11")
        Mouse.press(MOUSE_LEFT);
    if (buttonCMD == "mouse10")
        Mouse.release(MOUSE_LEFT);
    if (buttonCMD == "mouse21")
        Mouse.press(MOUSE_RIGHT);
    if (buttonCMD == "mouse20")
        Mouse.release(MOUSE_RIGHT);
    if (buttonCMD == "mouse41")
        Mouse.press(MOUSE_MIDDLE);
    if (buttonCMD == "mouse40")
        Mouse.release(MOUSE_MIDDLE);

    //Serial.println(temp);
    /*Serial.print(vx_out);
    Serial.print(" ");
    Serial.println(vy_out);*/
}

void buttonStates ()
{

    mouse1State = digitalRead(mouse1);
    mouse2State = digitalRead(mouse2);
    mouse3State = digitalRead(mouse3);
    mouse4State = digitalRead(mouse4);

    if (mouse1State != mouse1LastState)
    {
        if (mouse1State == HIGH)   // State changed from 0 to 1
        {
            buttonCMD = "mouse11";
            Serial.println(buttonCMD);
        }
        if (mouse1State == LOW)   // State changed from 1 to 0
        {
            buttonCMD = "mouse10";
            Serial.println(buttonCMD);
        }
    }
    if (mouse2State != mouse2LastState)
    {
        if (mouse2State == HIGH)
        {
            buttonCMD = "mouse21";
            Serial.println(buttonCMD);
        }
        if (mouse2State == LOW)
        {
            buttonCMD = "mouse20";
            Serial.println(buttonCMD);
        }
    }
    if (mouse3State != mouse3LastState)
    {
        if (mouse3State == HIGH)
        {
            mouse3PressCount++;
            if (mouse3PressCount == 4)
            {
                mouse3PressCount = 0;
            }
            switch (mouse3PressCount)
            {
                case 0:
                    sensitivityCAL = 1.5;
                    break;
                case 1:
                    sensitivityCAL = 3;
                    break;
                case 2:
                    sensitivityCAL = 6;
                    break;
                case 3:
                    sensitivityCAL = 12;
                    break;
            }
            temp = sensitivityCAL;
        }

    }
    if (mouse4State != mouse4LastState)
    {
        if (mouse4State == HIGH)
        {
            buttonCMD = "mouse41";
            Serial.println(buttonCMD);

        }
        if (mouse4State == LOW)
        {
            buttonCMD = "mouse40";
            Serial.println(buttonCMD);
        }
    }

    mouse1LastState = mouse1State;
    mouse2LastState = mouse2State;
    mouse3LastState = mouse3State;
    mouse4LastState = mouse4State;

}












