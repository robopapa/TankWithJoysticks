#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
#define JOYSTICK_L_PUSH_BUTTON 5
#define JOYSTICK_L_VERTICLE A0
#define JOYSTICK_L_HORIZONTAL A1
#define JOYSTICK_R_PUSH_BUTTON 4
#define JOYSTICK_R_VERTICLE A2
#define JOYSTICK_R_HORIZONTAL A3
#define CONNECT_LED 6 

const byte pipe[][6] = {"00006"};
RF24 radio(CE_PIN, CSN_PIN);
typedef struct {
  int vPosition;
  int hPosition;
  bool button;
} Joystick;
Joystick joystick[2]; 
void setup() {
  // put your setup code here, to run once:
  pinMode(JOYSTICK_L_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(JOYSTICK_R_PUSH_BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe[0]);
  radio.stopListening();
  digitalWrite(CONNECT_LED,HIGH);
    
}

void loop() {
  //if(radio.available()) {
    int push_l = digitalRead(JOYSTICK_L_PUSH_BUTTON);
    int ver_l = analogRead(JOYSTICK_L_VERTICLE);
    int hor_l = analogRead(JOYSTICK_L_HORIZONTAL);
    int push_r = digitalRead(JOYSTICK_R_PUSH_BUTTON);
    int ver_r = analogRead(JOYSTICK_R_VERTICLE);
    int hor_r = analogRead(JOYSTICK_R_HORIZONTAL);
    Serial.println("Value receieved");
    Serial.println("-----------------");
    Serial.print(push_l);
    Serial.print("\t");
    Serial.print(ver_l);
    Serial.print("\t");
    Serial.println(hor_l);
    joystick[0].button = push_l == HIGH ? false : true;
    joystick[0].vPosition = ver_l;
    joystick[0].hPosition = hor_l;
    joystick[1].button = push_r == HIGH ? false : true;
    joystick[1].vPosition = ver_r;
    joystick[1].hPosition = hor_r;
    radio.write(&joystick,sizeof(Joystick)*2);
}
