
// Define pins for photoresistors
#define LDR_TOP 0
#define LDR_BOTTOM 1
#define LDR_LEFT 2
#define LDR_RIGHT 3

// Define pins for stepper motors
#define MOTOR_X_DIR 4
#define MOTOR_X_STEP 5
#define MOTOR_Y_DIR 6
#define MOTOR_Y_STEP 7

// Define pins for endstops
#define ENDSTOP_X 8
#define ENDSTOP_Y 9

// Define steps per revolution for stepper motors
#define STEPS_PER_REV 200

// Define threshold for difference between photoresistors
#define THRESHOLD 50

// Define delay for stepper motors
#define DELAY 10

// Define variables for storing photoresistor values
int topValue, bottomValue, leftValue, rightValue;

// Define variables for storing stepper motor positions
int xPos, yPos;

// Define variables for storing endstop states
bool xEndstop, yEndstop;

// Define a JSON object for sending data over serial port
#include <ArduinoJson.h>
StaticJsonDocument<200> doc;

void setup() {
  // Initialize serial port
  Serial.begin(9600);

  // Set pins for stepper motors as outputs
  pinMode(MOTOR_X_DIR, OUTPUT);
  pinMode(MOTOR_X_STEP, OUTPUT);
  pinMode(MOTOR_Y_DIR, OUTPUT);
  pinMode(MOTOR_Y_STEP, OUTPUT);

  // Set pins for endstops as inputs with pullup resistors
  pinMode(ENDSTOP_X, INPUT_PULLUP);
  pinMode(ENDSTOP_Y, INPUT_PULLUP);

  // Home the axes by moving them until they hit the endstops
  homeX();
  homeY();
}

void loop() {
  // Read the values from the photoresistors
  topValue = analogRead(LDR_TOP);
  bottomValue = analogRead(LDR_BOTTOM);
  leftValue = analogRead(LDR_LEFT);
  rightValue = analogRead(LDR_RIGHT);

  // Compare the values and move the axes accordingly
  if (topValue - bottomValue > THRESHOLD) {
    // Move the Y axis up
    moveY(1);
    yPos++;
  }
  else if (bottomValue - topValue > THRESHOLD) {
    // Move the Y axis down
    moveY(-1);
    yPos--;
  }
  
  if (leftValue - rightValue > THRESHOLD) {
    // Move the X axis left
    moveX(-1);
    xPos--;
  }
  else if (rightValue - leftValue > THRESHOLD) {
    // Move the X axis right
    moveX(1);
    xPos++;
  }

  // Send the position of the sun over serial port in JSON format
  doc["x"] = xPos;
  doc["y"] = yPos;
  
  serializeJson(doc, Serial);
  
}

// Function to move the X axis by one step in a given direction
void moveX(int dir) {
  
   // Set the direction pin according to the direction parameter
   digitalWrite(MOTOR_X_DIR, dir > 0 ? HIGH : LOW);

   // Pulse the step pin to move one step
   digitalWrite(MOTOR_X_STEP, HIGH);
   delay(DELAY);
   digitalWrite(MOTOR_X_STEP, LOW);
   delay(DELAY); 
}

// Function to move the Y axis by one step in a given direction
void moveY(int dir) {
  
   // Set the direction pin according to the direction parameter
   digitalWrite(MOTOR_Y_DIR, dir > 0 ? HIGH : LOW);

   // Pulse the step pin to move one step
   digitalWrite(MOTOR_Y_STEP, HIGH);
   delay(DELAY);
   digitalWrite(MOTOR_Y_STEP, LOW);
   delay(DELAY); 
}

// Function to home the X axis by moving it until it hits the endstop
void homeX() {
  
   // Read the state of the endstop pin
   xEndstop = digitalRead(ENDSTOP_X);

   // While the endstop is not triggered, move the X axis left
   while (xEndstop == HIGH) {
     moveX(-1);
     xEndstop = digitalRead(ENDSTOP_X);
   }

   // Set the X position to zero
   xPos = 0;
}

// Function to home the Y axis by moving it until it hits the endstop
void homeY() {
  
   // Read the state of the endstop pin
   yEndstop = digitalRead(ENDSTOP_Y);

   // While the endstop is not triggered, move the Y axis down
   while (yEndstop == HIGH) {
     moveY(-1);
     yEndstop = digitalRead(ENDSTOP_Y);
   }

   // Set the Y position to zero
   yPos = 0;
}
