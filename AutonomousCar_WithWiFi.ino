// Arduino Pro Micro Code
// Libraries
#include <Servo.h>
#include <SoftwareSerial.h>
//=================================
// SoftwareSerial pins object
SoftwareSerial SerialToESP(RX_NUM, TX_NUM); // RX, TX
//=================================
// Motor 1 Pins
const int motor_RIGHT = 15;
const int motor_LEFT = 14;
// Motor 2 Pins
const int motor_FORWARD = 16;
const int motor_BACKWARD = 10;
//=================================
// Ultrasonic Sensor Pins
const int trig1 = 2;
const int echo1 = 3;
// Ultrasonc Sensor 2 Pins
const int trig2 = 4;
const int echo2 = 5;
//=================================
// Servo Motor 1 Pin
Servo servo1;
const int servo1_PIN = 8;
//=================================
// Definitions
#define brakingDistance 13
#define motor_SPEED_PIN 9
#define motor_DRIVE_SPEED 100

//=================================
// Ultrasonic Sensor Variables
long duration1 = 0;
int distance1 = 0;

long duration2 = 0;
int distance2 = 0;

int brakingCounter = 0;
int afterStopCounter = 0;

int totalRecalcDistance1 = 0;
boolean perfectDist1 = false;

//End of variables
//=================================
//=================================
/*
-----------------------------------
FUNCTION ORDER:
-----------------------------------
SETUP FUNCTIONS
  ┗━ setup_Ultrasonic_FRONT
  ┗━ setup_Ultrasonic_SIDE
  ┗━ setup_Servo_1
  ┗━ setup_Motors
  ┗━ setup_SerialToESP

DRIVE FUNCTIONS
  ┗━ setFORWARD_SPEED
  ┗━ start_FORWARD
  ┗━ drive_Motor_FORWARD
  ┗━ drive_Motor_BACKWARD
  ┗━ drive_Motor_RIGHT
  ┗━ drive_Motor_LEFT
  ┗━ slowAndStop_FORWARD

TEST FUNCTIONS
  ┗━ test_Motors
  ┗━ test_Servo
  ┗━ test_SerialToESP

DISTANCE CALCULATION FUNCTIONS
  ┗━ calculate_Distance_FRONT
  ┗━ calculate_Distance_SIDE
  ┗━ recalculateDistance1
  ┗━ lockInDistance

PRINTING FUNCTIONS
  ┗━ sendToESP
  ┗━ print_ultrasonic_data
-----------------------------------
THINGS TO ADD:
-----------------------------------
Pins for SerialToESP connection
Add things to do during driving delays like communication
  ┗━ Wasted time during delays
Fix turning time around objects
  ┗━ Add second UT turning direction decision (R or L)
Tweak time spent turning with second UT
Make second script for ESP8266

*/
//=================================
//=================================
// SETUP FUNCTIONS

void setup_Ultrasonic_FRONT() {
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
}

void setup_Ultrasonic_SIDE() {
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
}

void setup_Servo_1() {
  servo1.attach(servo1_PIN);
  servo1.write(180);
  delay(300);
  servo1.write(0);
  delay(300);
  servo1.write(180);
  delay(300);
}
void setup_Motors() {
  pinMode(motor_RIGHT,    OUTPUT);
  pinMode(motor_LEFT,     OUTPUT);
  pinMode(motor_FORWARD,  OUTPUT);
  pinMode(motor_BACKWARD, OUTPUT);
}
void setup_SerialToESP() {
  SerialToESP.begin(115200);
  delayMicroseconds(10);
}
//=================================
//=================================
// DRIVE FUNCTIONS

void setFORWARD_SPEED(int speed) {
  // 0-255
  analogWrite(motor_SPEED_PIN, speed);
}
//=================================
void start_FORWARD(int speed) {
  digitalWrite(motor_FORWARD, HIGH);
  analogWrite(motor_SPEED_PIN, speed);
}
//=================================
void drive_Motor_FORWARD(int time) {
  digitalWrite(motor_FORWARD, HIGH);
  delay( time );
  digitalWrite(motor_FORWARD, LOW);
}
//=================================
void drive_Motor_BACKWARD(int time) {
  digitalWrite(motor_BACKWARD, HIGH);
  delay( time );
  digitalWrite(motor_BACKWARD, LOW);
}
//=================================
void drive_Motor_RIGHT(int time) {
  digitalWrite(motor_RIGHT, HIGH);
  delay( time );
  digitalWrite(motor_RIGHT, LOW);
}
//=================================
void drive_Motor_LEFT(int time) {
  digitalWrite(motor_LEFT, HIGH);
  delay( time );
  digitalWrite(motor_LEFT, LOW);
}
//=================================
void slowAndStop_FORWARD() {
  digitalWrite(motor_FORWARD, LOW);
  drive_Motor_BACKWARD(20);
}
//=================================
//=================================
// TEST FUNCTIONS
void test_Motors() {
  digitalWrite(motor_RIGHT, HIGH);
  digitalWrite(motor_LEFT, LOW);

  digitalWrite(motor_FORWARD, HIGH);
  digitalWrite(motor_BACKWARD, LOW);
  delay(1000);

  digitalWrite(motor_RIGHT, LOW);
  digitalWrite(motor_LEFT, HIGH);

  digitalWrite(motor_FORWARD, LOW);
  digitalWrite(motor_BACKWARD, HIGH);
  delay(1000);
}

void test_Servo(int selector) {
  if ( selector == 1 ) {
    servo1.write(0);
    delay(500);
    servo1.write(300);
    delay(500);
    servo1.write(0);
    delay(500);
    servo1.write(300);
  } else if ( selector == 2 ) {
    // servo2.write(0);
    // delay(500);
    // servo2.write(170);
    // delay(500);
    // servo2.write(0);
    // delay(500);
    // servo2.write(170);
  }
}

boolean test_SerialToESP() {
  if ( SerialToESP.isListening() ) {
    SerialToESP.write(00111110);
    SerialToESP.print('T');
    SerialToESP.write(00100101);
    return(TRUE);
  } else {
    return(FALSE);
  }
}
//=================================
//=================================
// DISTANCE CALCULATIONS

void calculate_Distance_FRONT() {
  // Clears the trig1 condition
  digitalWrite(trig1, LOW);
  delayMicroseconds(2);
  // Sets the trig1 HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  // Reads the echo1, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echo1, HIGH);
  // Calculating the distance
  distance1 = (duration1 * 0.0343) / 2; // Speed of sound wave divided by 2 (go and back)
}
//=================================
// NEED TO ADD SERVO SYSTEM & ADD TO OTHER BOARD CODE
void calculate_Distance_SIDE() {
  // Clears the trig1 condition
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  // Sets the trig1 HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  // Reads the echo1, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echo2, HIGH);
  // Calculating the distance
  distance2 = (duration2 * 0.0343) / 2; // Speed of sound wave divided by 2 (go and back)
}

void recalculateDistance1() {
  totalRecalcDistance1 = 0;
  for ( int i=0; i < 3; i++ ) {
    calculate_Distance_FRONT();
    totalRecalcDistance1 += distance1;
  }
  totalRecalcDistance1 /= 3;
}

void lockInDistance() {
  while ( perfectDist1 == false ) {
    // Takes average of 3 distance calculations from Ultrasonic 1
    recalculateDistance1();
    // Serial.println(totalRecalcDistance1);

    if ( totalRecalcDistance1 == brakingDistance ||
         totalRecalcDistance1 == brakingDistance + 1 ||
         totalRecalcDistance1 == brakingDistance - 1 ) {
      // Turn Around Object ( Perfect Breaking Distance )
      perfectDist1 = true;

    } else if ( totalRecalcDistance1 > brakingDistance + 1 ) {
      // Car too far ( Move closer to object )
      drive_Motor_FORWARD(10);

    } else if ( totalRecalcDistance1 < brakingDistance - 1 ) {
      // Car too close ( Back up from object )
      drive_Motor_BACKWARD(10);
    }
  }
}
//=================================
//=================================
// PRINTING FUNCTIONS

// WILL BECOME OBSOLETE AFTER sendToESP IS FINISHED INTEGRATION
void sendToESP(String str) {
  // SerialToESP.print("%->"); // Start of packet identifier
  SerialToESP.write(00111110);
  SerialToESP.print(str);
  SerialToESP.write(00100101);
  // SerialToESP.print("<-%"); // End of packet identifier
}

void print_ultrasonic_data(int selector) {
  if ( selector == 1 ) {
    Serial.print(distance1);
    Serial.print(" ");
    for ( int i=distance1; i>=0; i-- ) {
      Serial.print("=");
    }
    Serial.println();
  } else if ( selector == 2 ) {
    Serial.print(distance2);
//    Serial.print(" ");
//    for ( int i=distance2; i>=0; i-- ) {
//      Serial.print("=");
//    }
    Serial.println();
  }
}
//=================================
//=================================

void setup() {
  Serial.begin(9600);

  // Setup Hardware
  setup_Motors();
  setup_Ultrasonic_FRONT();
//  setup_Ultrasonic_SIDE();
//  setup_Servo_1();


  // Start Car
  start_FORWARD();
//  test_Servo(1);
}

void loop() {
  calculate_Distance_FRONT();
//  calculate_Distance_SIDE();
  start_FORWARD();
  print_ultrasonic_data(1);
//  print_ultrasonic_data(2);



  if ( distance1 < brakingDistance ) {
    brakingCounter++;
  } else {
    // Reset brakingCounter
    brakingCounter = 0;
  }

  if ( brakingCounter >= 10 ) {
    // Stop Car using counter rotation
    slowAndStop_FORWARD();
    // Lock in distance from object
    lockInDistance();

    if ( perfectDist1 == true ) {
      // Turn right of object
      start_FORWARD(50);
      drive_Motor_RIGHT(4000)

      // Turn parallel to initial direction
      drive_Motor_LEFT(1000);
      digitalWrite(motor_FORWARD, HIGH);
      perfectDist1 = false;
    }
  }
}
