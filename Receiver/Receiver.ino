/**************************************************************************************************************/
/******** This code can be used with anything you need. However, it was written for this Tank in mind *********/
/******** https://www.sparkfun.com/products/retired/12092                                             *********/
/**************************************************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h" 

/************************************************/
/****** Declare Constants and Pin Numbers *******/
/************************************************/
#define CE_PIN          5
#define CSN_PIN         6
#define LEFT_JOYSTICK   0
#define RIGHT_JOYSTICK  1
#define SERVO_PIN       9
#define BREAKE          0
/*********** Adafruit Motor Shield V2.3 ************/

Adafruit_MotorShield AFMS     = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor   = AFMS.getMotor(3);
Adafruit_DCMotor *rightMotor  = AFMS.getMotor(4);

/***************************************************/

/******************* Servo setup *******************/

Servo liftupServo;

/**************************************************/
/******************* Radio setup ******************/

const byte pipe[][6] = {"00006"}; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN);      // Create a Radio

/**************************************************/

/*************** Transmitted Values ***************/

typedef struct {
  int vPosition;
  int hPosition;
  bool button;
} Joystick;
Joysticks joystick[2];

/**************************************************/

/**************** Global Variables ****************/
const bool Pressed       = true;
const bool Released      = false;
int leftMotorDirection   = FORWARD;
int rightMotorDirection  = FORWARD;
bool isLiftUp            = false;
bool buttonStatus        = false;
/**************************************************/

void setup()   
{
  Serial.begin(9600);
  delay(1000);
  Serial.println("Nrf24L01 Receiver Starting");
  radio.begin();
  radio.openReadingPipe(0,pipe[0]);
  radio.startListening();
  
  liftupServo.attach(SERVO_PIN); // can also use 8
  AFMS.begin();
}

void loop()
{
  
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    int val[2];
    while (!done)
    {
      radio.read(&joysticks,sizeof(Joystick)*2);
      // now we have the data that was transmitted, so we need to do some logic.
      if(joysticks[RIGHT_JOYSTICK].button == 1 && buttonStatus == Released) {
          buttonStatus  = Pressed;
          isLiftUp          = true;
      } else if (joysticks[RIGHT_JOYSTICK].button == 0 && buttonStatus == Pressed) {
        buttonStatus = Released;
      }

      if(isLiftup) {
        ProcessLiftUp();
      } else {
        ProcessMovement();
      }


#ifdef __DEBUG__
      PrintValues();
#endif
      delay(250);
      done = radio.available();
    }
  }
  else
  {    
      Serial.println("No radio available");
      delay(250);
  }
  
}

void ProcessMovement() {
  // Left Motor
  int value = joysticks[LEFT_JOYSTICK].vPosition;
  leftMotorDirection = GetDirection(value);
  int speed = GetSpeed(value, leftMotorDirectionFORWARD);
  leftMotor->setSpeed(speed);
  leftMotor->run(leftMotorDirection);

  // Right Motor
  value = joysticks[RIGHT_JOYSTICK].vPosition;
  rightMotorDirection = GetDirection(value);
  speed = GetSpeed(value, rightMotorDirection);
  rightMotor->setSpeed(speed);
  rightMotor->run(leftMotorDirection);
}

int GetDirection(int value) {
  return value < 512 ? BACKWARD : FORWARD;
}

int GetSpeed(int value, int direction) {
  return 
        direction == FORWARD ? 
        map ( value, 512, 1024, 0, 255 ) : 
        map ( value, 0, 512, 255, 0 );
}

void ProcessLiftUp(){
  // stop movement 
  leftMotor->setSpeed(BREAKE);
  rightMotor->setSpeed(BREAKE);
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);

  int value = joysticks[RIGHT_JOYSTICK].vPosition;
  int angle = map(value, 512, 1024, 0, 180);
  liftupServo.write(angle);
}

void PrintValues() {
  Serial.print("Left Joystick:\t");
  Serial.print(joystick[LEFT_JOYSTICK].vPosition);
  Serial.print("\t");
  Serial.print(joystick[LEFT_JOYSTICK].hPosition);
  Serial.print("\t");
  Serial.println(joystick[LEFT_JOYSTICK].button);
  Serial.print("Right Joystick\t");
  Serial.print(joystick[RIGHT_JOYSTICK].vPosition);
  Serial.print("\t");
  Serial.print(joystick[RIGHT_JOYSTICK].hPosition);
  Serial.print("\t");
  Serial.println(joystick[RIGHT_JOYSTICK].button);
}