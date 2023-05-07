
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

// Define the threshold for the difference between photoresistors
#define THRESHOLD 50

// Define the delay for the stepper motors
#define DELAY 10

void setup() {
  // Set the motor pins as outputs
  pinMode(MOTOR_X_DIR, OUTPUT);
  pinMode(MOTOR_X_STEP, OUTPUT);
  pinMode(MOTOR_Y_DIR, OUTPUT);
  pinMode(MOTOR_Y_STEP, OUTPUT);

  // Set the endstop pins as inputs with pullup resistors
  pinMode(ENDSTOP_X, INPUT_PULLUP);
  pinMode(ENDSTOP_Y, INPUT_PULLUP);

  // Initialize the serial monitor
  Serial.begin(9600);
}

void loop() {
  // Read the values from the photoresistors
  int top = analogRead(LDR_TOP);
  int bottom = analogRead(LDR_BOTTOM);
  int left = analogRead(LDR_LEFT);
  int right = analogRead(LDR_RIGHT);

  // Print the values to the serial monitor
  Serial.print("Top: ");
  Serial.print(top);
  Serial.print(" Bottom: ");
  Serial.print(bottom);
  Serial.print(" Left: ");
  Serial.print(left);
  Serial.print(" Right: ");
  Serial.println(right);

  // Calculate the difference between photoresistors
  int diff_x = left - right;
  int diff_y = top - bottom;

  // Move the x-axis motor if the difference is greater than the threshold and the endstop is not triggered
  if (abs(diff_x) > THRESHOLD && digitalRead(ENDSTOP_X) == HIGH) {
    // Set the direction of the motor based on the sign of the difference
    if (diff_x > 0) {
      digitalWrite(MOTOR_X_DIR, HIGH);
    } else {
      digitalWrite(MOTOR_X_DIR, LOW);
    }
    // Make one step of the motor
    digitalWrite(MOTOR_X_STEP, HIGH);
    delay(DELAY);
    digitalWrite(MOTOR_X_STEP, LOW);
    delay(DELAY);
  }

  // Move the y-axis motor if the difference is greater than the threshold and the endstop is not triggered
  if (abs(diff_y) > THRESHOLD && digitalRead(ENDSTOP_Y) == HIGH) {
    // Set the direction of the motor based on the sign of the difference
    if (diff_y > 0) {
      digitalWrite(MOTOR_Y_DIR, HIGH);
    } else {
      digitalWrite(MOTOR_Y_DIR, LOW);
    }
    // Make one step of the motor
    digitalWrite(MOTOR_Y_STEP, HIGH);
    delay(DELAY);
    digitalWrite(MOTOR_Y_STEP, LOW);
    delay(DELAY);
  }
}
