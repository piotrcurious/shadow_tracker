
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

// Define the steps per revolution for the motors
#define STEPS_PER_REV 200

// Define the learning rate for the gradient descent algorithm
#define LEARNING_RATE 0.01

// Define the threshold for the error function
#define ERROR_THRESHOLD 0.001

// Define the interval for finding the sun's position in milliseconds
#define INTERVAL 300000

// Initialize the variables for the axes positions
int x_pos = 0;
int y_pos = 0;

// Initialize the variables for the photoresistor readings
int top_reading = 0;
int bottom_reading = 0;
int left_reading = 0;
int right_reading = 0;

// Initialize the variable for the last update time
unsigned long last_update = 0;

void setup() {
  // Set the motor pins as outputs
  pinMode(MOTOR_X_DIR, OUTPUT);
  pinMode(MOTOR_X_STEP, OUTPUT);
  pinMode(MOTOR_Y_DIR, OUTPUT);
  pinMode(MOTOR_Y_STEP, OUTPUT);

  // Set the endstop pins as inputs with pullup resistors
  pinMode(ENDSTOP_X, INPUT_PULLUP);
  pinMode(ENDSTOP_Y, INPUT_PULLUP);

  // Start serial communication at 9600 baud rate
  Serial.begin(9600);

  // Home the axes by moving them until they hit the endstops
  home_axes();
}

void loop() {
  // Get the current time in milliseconds
  unsigned long current_time = millis();

  // Check if it is time to update the sun's position
  if (current_time - last_update >= INTERVAL) {
    // Update the last update time
    last_update = current_time;

    // Find the sun's position using gradient descent algorithm
    find_sun();

    // Send the axes positions over serial port in JSON format
    send_position();
  }
}

// Function to home the axes by moving them until they hit the endstops
void home_axes() {
  // Move the x-axis to the left until it hits the endstop
  while (digitalRead(ENDSTOP_X) == HIGH) {
    move_motor(MOTOR_X_DIR, MOTOR_X_STEP, -1);
    x_pos--;
  }

  // Move the y-axis to the bottom until it hits the endstop
  while (digitalRead(ENDSTOP_Y) == HIGH) {
    move_motor(MOTOR_Y_DIR, MOTOR_Y_STEP, -1);
    y_pos--;
  }

  // Reset the axes positions to zero
  x_pos = 0;
  y_pos = 0;
}

// Function to move a motor by one step in a given direction
void move_motor(int dir_pin, int step_pin, int dir) {
  // Set the direction pin according to the direction parameter
  if (dir > 0) {
    digitalWrite(dir_pin, HIGH);
  } else {
    digitalWrite(dir_pin, LOW);
  }

  // Pulse the step pin to move one step
  digitalWrite(step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(step_pin, LOW);
}

// Function to find the sun's position using gradient descent algorithm
void find_sun() {
  // Initialize the variables for the error and the gradients
  float error = 0;
  float x_grad = 0;
  float y_grad = 0;

  // Repeat until the error is below the threshold
  do {
    // Read the photoresistor values
    read_sensors();

    // Calculate the error as the sum of squared differences between opposite sensors
    error = sq(top_reading - bottom_reading) + sq(left_reading - right_reading);

    // Calculate the partial derivatives of the error with respect to x and y positions
    x_grad = -2 * (top_reading - bottom_reading) * (left_reading + right_reading);
    y_grad = -2 * (left_reading - right_reading) * (top_reading + bottom_reading);

    // Update the x and y positions by moving one step in the opposite direction of the gradients
    if (x_grad > 0) {
      move_motor(MOTOR_X_DIR, MOTOR_X_STEP, -1);
      x_pos--;
    } else if (x_grad < 0) {
      move_motor(MOTOR_X_DIR, MOTOR_X_STEP, 1);
      x_pos++;
    }

    if (y_grad > 0) {
      move_motor(MOTOR_Y_DIR, MOTOR_Y_STEP, -1);
      y_pos--;
    } else if (y_grad < 0) {
      move_motor(MOTOR_Y_DIR, MOTOR_Y_STEP, 1);
      y_pos++;
    }

  } while (error > ERROR_THRESHOLD);
}

// Function to read the photoresistor values and map them to a range from 0 to 100
void read_sensors() {
  top_reading = map(analogRead(LDR_TOP), 0, 1023, 0, 100);
  bottom_reading = map(analogRead(LDR_BOTTOM), 0, 1023, 0, 100);
  left_reading = map(analogRead(LDR_LEFT), 0, 1023, 0, 100);
  right_reading = map(analogRead(LDR_RIGHT), 0, 1023, 0, 100);
}

// Function to send the axes positions over serial port in JSON format
void send_position() {
  // Start the JSON object
  Serial.print("{");

  // Send the x position with a key of "x"
  Serial.print("\"x\":");
  Serial.print(x_pos);

  // Send a comma to separate the values
  Serial.print(",");

  // Send the y position with a key of "y"
  Serial.print("\"y\":");
  Serial.print(y_pos);

  // End the JSON object
  Serial.println("}");
}
