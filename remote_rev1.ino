#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// pin definitions
#define CE 7
#define CSN 8
#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define BUTTON 12

// parameters
const int padding_s = 100; // padding for joystick
const byte address[6] = "08079"; 

///////////////////////

const int padding_e = 1023 - padding_s;
int remote_data[3]; // [joystick_x (-255 to 255), joystick_y (-255 to 255), button (0 or 1)]
RF24 radio(CE, CSN);

void setup() {
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(BUTTON, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  // reading joystick values 
  int data_x = analogRead(JOYSTICK_X);
  data_x = constrain(data_x, padding_s, padding_e);
  remote_data[0] = map(data_x, padding_s, padding_e, -255, 255);
  int data_y = analogRead(JOYSTICK_Y);
  data_y = constrain(data_y, padding_s, padding_e);
  remote_data[1] = map(data_y, padding_s, padding_e, -255, 255);
  remote_data[2] = digitalRead(BUTTON);

  radio.write(&remote_data, sizeof(remote_data));
}