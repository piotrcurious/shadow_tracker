
// Define the pins for the photoresistors
#define LDR_TOP 0
#define LDR_BOTTOM 1
#define LDR_LEFT 2
#define LDR_RIGHT 3

// Define the pins for the stepper motors
#define MOTOR_X_DIR 4
#define MOTOR_X_STEP 5
#define MOTOR_Y_DIR 6
#define MOTOR_Y_STEP 7

// Define the pins for the endstops
#define ENDSTOP_X 8
#define ENDSTOP_Y 9

// Define the steps per revolution for the stepper motors
#define STEPS_PER_REV 200

// Define the minimum and maximum angles for the axes
#define MIN_ANGLE_X -45
#define MAX_ANGLE_X 45
#define MIN_ANGLE_Y -30
#define MAX_ANGLE_Y 30

// Define the threshold for the photoresistor difference
#define THRESHOLD 10

// Define the interval for finding the sun's position in milliseconds
#define INTERVAL 300000

// Define the global variables for the axes positions and angles
int posX = 0;
int posY = 0;
int angleX = 0;
int angleY = 0;

// Define the global variables for the photoresistor values
int topValue = 0;
int bottomValue = 0;
int leftValue = 0;
int rightValue = 0;

void setup() {
  // Set the motor pins as outputs
  pinMode(MOTOR_X_DIR, OUTPUT);
  pinMode(MOTOR_X_STEP, OUTPUT);
  pinMode(MOTOR_Y_DIR, OUTPUT);
  pinMode(MOTOR_Y_STEP, OUTPUT);

  // Set the endstop pins as inputs with pullup resistors
  pinMode(ENDSTOP_X, INPUT_PULLUP);
  pinMode(ENDSTOP_Y, INPUT_PULLUP);

  // Initialize the serial port at 9600 baud rate
  Serial.begin(9600);

  // Home the axes by moving them to the endstops
  homeX();
  homeY();
}

void loop() {
  // Read the photoresistor values and store them in variables
  topValue = analogRead(LDR_TOP);
  bottomValue = analogRead(LDR_BOTTOM);
  leftValue = analogRead(LDR_LEFT);
  rightValue = analogRead(LDR_RIGHT);

  // Find the sun's position using binary search algorithm
  findSun();

  // Send the axes angles over serial port in JSON format
  sendAngles();

  // Wait for the interval before repeating the loop
  delay(INTERVAL);
}

// Function to home the X axis by moving it to the endstop and resetting its position and angle
void homeX() {
  // Set the direction of the motor to move towards the endstop
  digitalWrite(MOTOR_X_DIR, LOW);

  // While the endstop is not triggered, move one step at a time
  while (digitalRead(ENDSTOP_X) == HIGH) {
    digitalWrite(MOTOR_X_STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(MOTOR_X_STEP, LOW);
    delayMicroseconds(1000);

    // Decrement the position by one step
    posX--;
  }

  // Reset the position and angle to zero
  posX = 0;
  angleX = 0;
}

// Function to home the Y axis by moving it to the endstop and resetting its position and angle
void homeY() {
   // Set the direction of the motor to move towards the endstop
   digitalWrite(MOTOR_Y_DIR, LOW);

   // While the endstop is not triggered, move one step at a time 
   while (digitalRead(ENDSTOP_Y) == HIGH) {
     digitalWrite(MOTOR_Y_STEP, HIGH);
     delayMicroseconds(1000);
     digitalWrite(MOTOR_Y_STEP, LOW);
     delayMicroseconds(1000);

     // Decrement the position by one step 
     posY--;
   }

   // Reset the position and angle to zero 
   posY = 0;
   angleY = 0;
}

// Function to find the sun's position using binary search algorithm 
void findSun() {
   // Define the variables for the lower and upper bounds of the search 
   int lowX = MIN_ANGLE_X;
   int highX = MAX_ANGLE_X;
   int lowY = MIN_ANGLE_Y;
   int highY = MAX_ANGLE_Y;

   // Define a variable for storing the midpoints of each search 
   int midX = (lowX + highX) /2;
   int midY = (lowY + highY) /2;

   // Repeat until both searches converge to a single angle 
   while (lowX < highX || lowY < highY) {
      // Find and move to the midpoint of X axis search 
      midX = (lowX + highX) /2;
      moveToAngleX(midX);

      // Read and compare the left and right photoresistor values 
      leftValue = analogRead(LDR_LEFT);
      rightValue = analogRead(LDR_RIGHT);

      // If left value is greater than right value plus threshold, set low bound to midpoint plus one 
      if (leftValue > rightValue + THRESHOLD) {
         lowX = midX +1;
      }
      // If right value is greater than left value plus threshold, set high bound to midpoint minus one 
      else if (rightValue > leftValue + THRESHOLD) {
         highX = midX -1;
      }
      // If both values are within threshold, set both bounds to midpoint 
      else {
         lowX = midX;
         highX = midX;
      }

      // Find and move to the midpoint of Y axis search 
      midY = (lowY + highY) /2;
      moveToAngleY(midY);

      // Read and compare the top and bottom photoresistor values 
      topValue = analogRead(LDR_TOP);
      bottomValue = analogRead(LDR_BOTTOM);

      // If top value is greater than bottom value plus threshold, set low bound to midpoint plus one 
      if (topValue > bottomValue + THRESHOLD) {
         lowY = midY +1;
      }
      // If bottom value is greater than top value plus threshold, set high bound to midpoint minus one 
      else if (bottomValue > topValue + THRESHOLD) {
         highY = midY -1;
      }
      // If both values are within threshold, set both bounds to midpoint 
      else {
         lowY = midY;
         highY = midY;
      }
   }

   // Set angle variables to final values 
   angleX = midX;
   angleY = midY;   
}

// Function to move X axis motor to a given angle 
void moveToAngleX(int targetAngle) {
   // Calculate how many steps are needed to reach target angle from current angle 
   int stepsNeeded = (targetAngle - angleX) * STEPS_PER_REV /360;

   // If steps needed are positive, set direction to clockwise 
   if (stepsNeeded >0) {
      digitalWrite(MOTOR_X_DIR, HIGH);
   }
   // If steps needed are negative, set direction to counterclockwise and make steps positive 
   else if (stepsNeeded <0) {
      digitalWrite(MOTOR_X_DIR, LOW);
      stepsNeeded *= -1;      
   }

   // Move motor by steps needed with a delay between each step 
   for (int i=0; i<stepsNeeded; i++) {
     digitalWrite(MOTOR_X_STEP, HIGH);
     delayMicroseconds(1000);
     digitalWrite(MOTOR_X_STEP, LOW);
     delayMicroseconds(1000);

     // Increment or decrement position by one step depending on direction 
     if (digitalRead(MOTOR_X_DIR) == HIGH) {
        posX++;
     }
     else {
        posX--;
     }
   }
}

// Function to move Y axis motor to a given angle 
void moveToAngleY(int targetAngle) {
   // Calculate how many steps are needed to reach target angle from current angle 
   int stepsNeeded = (targetAngle - angleY) * STEPS_PER_REV /360;

   // If steps needed are positive, set direction to clockwise 
   if (stepsNeeded >0) {
      digitalWrite(MOTOR_Y_DIR, HIGH);
   }
   // If steps needed are negative, set direction to counterclockwise and make steps positive 
   else if (stepsNeeded <0) {
      digitalWrite(MOTOR_Y_DIR, LOW);
      stepsNeeded *= -1;      
   }

   // Move motor by steps needed with a delay between each step 
   for (int i=0; i<stepsNeeded; i++) {
     digitalWrite(MOTOR_Y_STEP, HIGH);
     delayMicroseconds(1000);
     digitalWrite(MOTOR_Y_STEP, LOW);
     delayMicroseconds(1000);

     // Increment or decrement position by one step depending on direction 
     if (digitalRead(MOTOR_Y_DIR) == HIGH) {
        posY++;
     }
     else {
        posY--;
     }
   }
}

// Function to send axes angles over serial port in JSON format 
void sendAngles() {
    Serial.print("{\"angle_x\":");
    Serial.print(angleX);    
    Serial.print(",\"angle_y\":");
    Serial.print(angleY);    
    Serial.println("}");
}

