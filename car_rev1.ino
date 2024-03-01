# courtesy : Binit Podddar


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// pin definitions
#define CE 7
#define CSN 8

// left motor
#define ENA 1
#define IN1 2
#define IN2 3
// right motor
#define IN3 4
#define IN4 5
#define ENB 6

#define SOLENOID 13

// parameters
const int solenoid_delay = 50; // time to keep solenoid on (ms)
const byte address[6] = "08079";
// const int movement_threshold = 15; // between 0-255, threshold under which to treat as unidirectional movement 

/////////////////////
/*
 control scheme:
 ******* ASSUMING Y +ve is UP and X +ve is RIGHT (will need to tune remote code accordingly) *******

 on only x or only y movement of joystick, car should rotate in-place (on x movement) or go straight fwd or bkd (on y movement)
 -- by 'only' I mean the other axis be close to normal position

 when both x and y are away from normal position (say the joystick is in diagonal position):
 car should go in arc-like motion, according to joystick's direction
*/

int data[3];
int x_axis = 0;
int y_axis = 0;
int kick = 0;
int kick_last = 0;
RF24 radio(CE, CSN);

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(SOLENOID, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(SOLENOID, LOW);

  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void setMotorSpeeds(int speed_a, int speed_b) { // meant to also be used to control direction using -ve speed
  if(speed_a >= 0) { // set speed and set motor a fwd
    analogWrite(ENA, speed_a);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else { // set motor a bkd
    analogWrite(ENA, -speed_a);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  if(speed_b >= 0) { // set speed and set motor a fwd
    analogWrite(ENB, speed_b);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else { // set motor a bkd
    analogWrite(ENB, -speed_b);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));

    x_axis = data[0];
    y_axis = data[1];
    kick = data[2];
  }

  if(kick_last == 0 and kick == 1) {
    // kick sequence
    digitalWrite(SOLENOID, HIGH);
    delay(solenoid_delay);
    digitalWrite(SOLENOID, LOW);
    kick_last = 1;
  }
  if(kick_last == 1 and kick == 0) {
    kick_last = 0;
  }

  int speed_a_y = y_axis;
  int speed_b_y = y_axis;
  int speed_a_x = x_axis;
  int speed_b_x = -x_axis;
  int speed_a = constrain(speed_a_x + speed_a_y, -255, 255);
  int speed_b = constrain(speed_b_x + speed_b_y, -255, 255);

  setMotorSpeeds(speed_a, speed_b);

}
