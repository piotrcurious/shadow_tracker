
// Define the pins for the photoresistors
#define LDR_TOP 0
#define LDR_BOTTOM 1
#define LDR_LEFT 2
#define LDR_RIGHT 3

// Define the pins for the stepper motors
#define X_STEP 4
#define X_DIR 5
#define X_EN 6
#define Y_STEP 7
#define Y_DIR 8
#define Y_EN 9

// Define the pins for the endstops
#define X_ENDSTOP 10
#define Y_ENDSTOP 11

// Define the parameters for the gradient descent algorithm
#define ALPHA 0.01 // Learning rate
#define EPSILON 0.001 // Convergence threshold
#define MAX_ITER 100 // Maximum number of iterations

// Define the parameters for the solar tracker system
#define X_MIN -90 // Minimum angle for the x-axis in degrees
#define X_MAX 90 // Maximum angle for the x-axis in degrees
#define Y_MIN -45 // Minimum angle for the y-axis in degrees
#define Y_MAX 45 // Maximum angle for the y-axis in degrees
#define STEP_ANGLE 1.8 // Angle per step for the stepper motors in degrees
#define DELAY_TIME 1000 // Delay time between steps in milliseconds
#define INTERVAL_TIME 300000 // Interval time between sun position updates in milliseconds

// Define some global variables
int x_pos = 0; // Current position of the x-axis in degrees
int y_pos = 0; // Current position of the y-axis in degrees
unsigned long last_time = 0; // Last time the sun position was updated

void setup() {
  // Initialize the serial port
  Serial.begin(9600);

  // Initialize the pins for the stepper motors as outputs and enable them
  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  pinMode(X_EN, OUTPUT);
  digitalWrite(X_EN, LOW);
  pinMode(Y_STEP, OUTPUT);
  pinMode(Y_DIR, OUTPUT);
  pinMode(Y_EN, OUTPUT);
  digitalWrite(Y_EN, LOW);

  // Initialize the pins for the endstops as inputs with pull-up resistors
  pinMode(X_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_ENDSTOP, INPUT_PULLUP);

  // Home the axes by moving them until they hit the endstops
  home_axes();
}

void loop() {
  // Check if it is time to update the sun position
  unsigned long current_time = millis();
  if (current_time - last_time >= INTERVAL_TIME) {
    // Update the last time variable
    last_time = current_time;

    // Find the sun position using gradient descent algorithm
    find_sun_position();
    
    // Send the sun position over serial port in JSON format
    send_sun_position();
    
    // Move the axes to align with the sun position
    move_axes();
    
    }
}

// Function to home the axes by moving them until they hit the endstops
void home_axes() {
  // Set the direction of the x-axis to negative
  digitalWrite(X_DIR, LOW);

  // Move the x-axis until it hits the endstop
  while (digitalRead(X_ENDSTOP) == HIGH) {
    // Make one step
    digitalWrite(X_STEP, HIGH);
    delayMicroseconds(DELAY_TIME);
    digitalWrite(X_STEP, LOW);
    delayMicroseconds(DELAY_TIME);
  }

  // Set the position of the x-axis to the minimum value
  x_pos = X_MIN;

  // Set the direction of the y-axis to negative
  digitalWrite(Y_DIR, LOW);

  // Move the y-axis until it hits the endstop
  while (digitalRead(Y_ENDSTOP) == HIGH) {
    // Make one step
    digitalWrite(Y_STEP, HIGH);
    delayMicroseconds(DELAY_TIME);
    digitalWrite(Y_STEP, LOW);
    delayMicroseconds(DELAY_TIME);
  }

  // Set the position of the y-axis to the minimum value
  y_pos = Y_MIN;
}

// Function to find the sun position using gradient descent algorithm
void find_sun_position() {
  // Initialize some variables for the algorithm
  int x_old = x_pos; // Previous position of the x-axis in degrees
  int y_old = y_pos; // Previous position of the y-axis in degrees
  int x_new = x_old; // New position of the x-axis in degrees
  int y_new = y_old; // New position of the y-axis in degrees
  int iter = 0; // Number of iterations
  float error = 0; // Error value

  // Repeat until convergence or maximum number of iterations is reached
  do {
    // Update the previous positions
    x_old = x_new;
    y_old = y_new;

    // Calculate the partial derivatives of the error function with respect to x and y
    float dx = partial_derivative(x_old, y_old, true);
    float dy = partial_derivative(x_old, y_old, false);

    // Update the new positions using the gradient descent formula
    x_new = x_old - ALPHA * dx;
    y_new = y_old - ALPHA * dy;

    // Constrain the new positions to the minimum and maximum values
    x_new = constrain(x_new, X_MIN, X_MAX);
    y_new = constrain(y_new, Y_MIN, Y_MAX);

    // Calculate the error value using the mean squared error function
    error = mean_squared_error(x_new, y_new);

    // Increment the number of iterations
    iter++;

  } while (error > EPSILON && iter < MAX_ITER);

  // Update the global variables for the sun position
  x_pos = x_new;
  y_pos = y_new;
}


// Function to calculate the partial derivative of the error function with respect to x or y
float partial_derivative(int x, int y, bool is_x) {
  // Initialize some variables for the calculation
  float delta = 0.01; // Small change in the variable
  float e1 = 0; // Error value before the change
  float e2 = 0; // Error value after the change
  float d = 0; // Partial derivative value

  // Calculate the error value before the change
  e1 = mean_squared_error(x, y);

  // Change the variable by a small amount depending on whether it is x or y
  if (is_x) {
    x += delta;
  } else {
    y += delta;
  }

  // Calculate the error value after the change
  e2 = mean_squared_error(x, y);

  // Calculate the partial derivative using the difference quotient formula
  d = (e2 - e1) / delta;

  // Return the partial derivative value
  return d;
}


// Function to calculate the mean squared error function
float mean_squared_error(int x, int y) {
  // Initialize some variables for the calculation
  float e = 0; // Error value
  float s = 0; // Sum of squared errors
  int n = 4; // Number of photoresistors

  // Move the axes to the given position
  move_axes(x, y);

  // Read the values of the photoresistors
  int top = analogRead(LDR_TOP);
  int bottom = analogRead(LDR_BOTTOM);
  int left = analogRead(LDR_LEFT);
  int right = analogRead(LDR_RIGHT);

  // Calculate the errors for each photoresistor
  float e_top = top - bottom;
  float e_bottom = bottom - top;
  float e_left = left - right;
  float e_right = right - left;

  // Square the errors and add them to the sum
  s += e_top * e_top;
  s += e_bottom * e_bottom;
  s += e_left * e_left;
  s += e_right * e_right;

  // Calculate the mean squared error by dividing the sum by the number of photoresistors
  e = s / n;

  // Return the error value
  return e;
}
