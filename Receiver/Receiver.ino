#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Servo.h>

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h" 

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   5
#define CSN_PIN 6
#define LEFT_JOYSTICK 0
#define RIGHT_JOYSTICK 1


const byte pipe[][6] = {"00006"}; // Define the transmit pipe

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);
Servo myservo;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

typedef struct {
  int vPosition;
  int hPosition;
  bool button;
} Joystick;

Joystick joystick[2];
void setup()   
{
  Serial.begin(9600);
  delay(1000);
  Serial.println("Nrf24L01 Receiver Starting");
  radio.begin();
  //radio.setPALevel(RF24_PA_LOW);
  //radio.setDataRate(RF24_2MBPS);
  //radio.setChannel(124);
  radio.openReadingPipe(0,pipe[0]);
  radio.startListening();
  //radio.printDetails();

  myservo.attach(9); // can also use 8
  AFMS.begin();
}

bool leftMotorForward = true;
bool rightMotorForward = true;
void loop()
{
  
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    int val[2];
    while (!done)
    {
      // Fetch the data payload
//      radio.read( &joysticks[0], sizeof(joysticks) );
//      radio.read( &joysticks[1], sizeof(joysticks) );
//      Serial.print("sizeof(joystick) = ");
//      Serial.println(sizeof(Joystick));
      radio.read(&joystick,sizeof(Joystick)*2);
      // PrintValues();
      delay(250);
    }
  }
  else
  {    
      Serial.println("No radio available");
      delay(250);
  }
  
}//--(end main loop )---

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

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********
