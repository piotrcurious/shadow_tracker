
// Define the pins for the photoresistors
#define LDR1 A0
#define LDR2 A1
#define LDR3 A2
#define LDR4 A3

// Define the pins for the stepper motors
#define X_STEP 2
#define X_DIR 3
#define Y_STEP 4
#define Y_DIR 5

// Define the pins for the endstops
#define X_ENDSTOP 6
#define Y_ENDSTOP 7

// Define the parameters for the binary search algorithm
#define TOLERANCE 10 // The maximum difference between the LDR readings to consider them equal
#define MAX_ITERATIONS 10 // The maximum number of iterations to perform in each axis

// Define the parameters for the stepper motors
#define STEP_DELAY 100 // The delay in microseconds between each step
#define STEPS_PER_REV 200 // The number of steps per revolution of the motor
#define DEG_PER_STEP 1.8 // The angle in degrees per step of the motor

// Define the parameters for the solar tracker system
#define X_MIN -90 // The minimum angle in degrees for the x-axis
#define X_MAX 90 // The maximum angle in degrees for the x-axis
#define Y_MIN -45 // The minimum angle in degrees for the y-axis
#define Y_MAX 45 // The maximum angle in degrees for the y-axis

// Declare global variables to store the current position of the axes
int x_pos = 0;
int y_pos = 0;

// Declare global variables to store the LDR readings
int ldr1 = 0;
int ldr2 = 0;
int ldr3 = 0;
int ldr4 = 0;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Set the pins for the photoresistors as inputs
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  pinMode(LDR3, INPUT);
  pinMode(LDR4, INPUT);

  // Set the pins for the stepper motors as outputs
  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  pinMode(Y_STEP, OUTPUT);
  pinMode(Y_DIR, OUTPUT);

  // Set the pins for the endstops as inputs with pull-up resistors
  pinMode(X_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_ENDSTOP, INPUT_PULLUP);

  // Home the axes on reset
  home();
}

void loop() {
  // Read the values from the photoresistors and store them in variables
  ldr1 = analogRead(LDR1);
  ldr2 = analogRead(LDR2);
  ldr3 = analogRead(LDR3);
  ldr4 = analogRead(LDR4);

  // Perform binary search on x-axis to find the sun's position
  binary_search_x();

  // Perform binary search on y-axis to find the sun's position
  binary_search_y();

  // Send the found position of the sun over serial port in JSON format
  Serial.print("{\"x\":");
  Serial.print(x_pos);
  Serial.print(",\"y\":");
  Serial.print(y_pos);
  Serial.println("}");

}


// Function to home the axes on reset
void home() {
  
   // Declare a local variable to store the number of steps taken 
   int steps = 0;

   // Move x-axis to negative direction until it hits the endstop or reaches the limit 
   while (digitalRead(X_ENDSTOP) == HIGH && steps < (X_MAX - X_MIN) * STEPS_PER_REV / DEG_PER_STEP) {
    move_x(-1);
    steps++;
   }

   // Reset x-axis position to zero 
   x_pos = X_MIN;

   // Reset steps count 
   steps = 0;

   // Move x-axis to positive direction until it is at the center 
   while (x_pos < (X_MIN + X_MAX) /2) {
    move_x(1);
   }

   // Move y-axis to negative direction until it hits the endstop or reaches the limit 
   while (digitalRead(Y_ENDSTOP) == HIGH && steps < (Y_MAX - Y_MIN) * STEPS_PER_REV / DEG_PER_STEP) {
    move_y(-1);
    steps++;
   }

   // Reset y-axis position to zero 
   y_pos = Y_MIN;

   // Reset steps count 
   steps = 0;

   // Move y-axis to positive direction until it is at the center 
   while (y_pos < (Y_MIN + Y_MAX) /2) {
    move_y(1);
   }
}


// Function to perform binary search on x-axis to find the sun's position 
void binary_search_x() {

   // Declare local variables to store the lower and upper bounds of the search range 
   int low = X_MIN;
   int high = X_MAX;

   // Declare a local variable to store the number of iterations 
   int iter = 0;

   // Declare a local variable to store the difference between LDR readings 
   int diff = abs(ldr1 - ldr2);

   // Loop until the difference is within tolerance or maximum iterations are reached 
   while (diff > TOLERANCE && iter < MAX_ITERATIONS) {

     // Calculate the mid point of the search range 
     int mid = (low + high) /2;

     // Move x-axis to mid point 
     while (x_pos != mid) {
       if (x_pos < mid) {
         move_x(1);
       }
       else {
         move_x(-1);
       }
     }

     // Read LDR values and calculate difference 
     ldr1 = analogRead(LDR1);
     ldr2 = analogRead(LDR2);
     diff = abs(ldr1 - ldr2);

     // Update search range based on difference 
     if (ldr1 > ldr2) {
       low = mid;
     }
     else {
       high = mid;
     }

     // Increment iteration count 
     iter++;
   }
}

// Function to perform binary search on y-axis to find the sun's position 
void binary_search_y() {

   // Declare local variables to store the lower and upper bounds of the search range 
   int low = Y_MIN;
   int high = Y_MAX;

   // Declare a local variable to store the number of iterations 
   int iter = 0;

   // Declare a local variable to store the difference between LDR readings 
   int diff = abs(ldr3 - ldr4);

   // Loop until the difference is within tolerance or maximum iterations are reached 
   while (diff > TOLERANCE && iter < MAX_ITERATIONS) {

     // Calculate the mid point of the search range 
     int mid = (low + high) /2;

     // Move y-axis to mid point 
     while (y_pos != mid) {
       if (y_pos < mid) {
         move_y(1);
       }
       else {
         move_y(-1);
       }
     }

     // Read LDR values and calculate difference 
     ldr3 = analogRead(LDR3);
     ldr4 = analogRead(LDR4);

     diff = abs(ldr3 - ldr4);

     // Update search range based on difference 
     if (ldr3 > ldr4) {
       low = mid;
     }
     else {
       high = mid;
     }

     // Increment iteration count 
     iter++;
   }
}

// Function to move x-axis by one step in the given direction 
void move_x(int dir) {

  // Check if the movement is within the limits 
  if (x_pos + dir >= X_MIN && x_pos + dir <= X_MAX) {

    // Set the direction pin according to the sign of dir 
    if (dir > 0) {
      digitalWrite(X_DIR, HIGH);
    }
    else {
      digitalWrite(X_DIR, LOW);
    }

    // Pulse the step pin to move one step 
    digitalWrite(X_STEP, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(X_STEP, LOW);
    delayMicroseconds(STEP_DELAY);

    // Update the x-axis position 
    x_pos += dir;
  }
}

// Function to move y-axis by one step in the given direction 
void move_y(int dir) {

  // Check if the movement is within the limits 
  if (y_pos + dir >= Y_MIN && y_pos + dir <= Y_MAX) {

    // Set the direction pin according to the sign of dir 
    if (dir > 0) {
      digitalWrite(Y_DIR, HIGH);
    }
    else {
      digitalWrite(Y_DIR, LOW);
    }

    // Pulse the step pin to move one step 
    digitalWrite(Y_STEP, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(Y_STEP, LOW);
    delayMicroseconds(STEP_DELAY);

    // Update the y-axis position 
    y_pos += dir;
  }
}

