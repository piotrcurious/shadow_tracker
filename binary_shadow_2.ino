
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

// Define the constants for the stepper motors
#define STEPS_PER_REV 200 // Number of steps per revolution
#define STEP_DELAY 2 // Delay between steps in milliseconds
#define X_MAX_ANGLE 90 // Maximum angle for the x-axis in degrees
#define Y_MAX_ANGLE 90 // Maximum angle for the y-axis in degrees
#define X_MIN_ANGLE -90 // Minimum angle for the x-axis in degrees
#define Y_MIN_ANGLE -90 // Minimum angle for the y-axis in degrees

// Define the variables for the stepper motors
int x_pos = 0; // Current position of the x-axis in degrees
int y_pos = 0; // Current position of the y-axis in degrees
int x_dir = 1; // Current direction of the x-axis (1 for clockwise, -1 for counterclockwise)
int y_dir = 1; // Current direction of the y-axis (1 for clockwise, -1 for counterclockwise)

// Define the variables for the photoresistors
int ldr_top = 0; // Current value of the top photoresistor
int ldr_bottom = 0; // Current value of the bottom photoresistor
int ldr_left = 0; // Current value of the left photoresistor
int ldr_right = 0; // Current value of the right photoresistor

// Define the variables for the binary search algorithm
int x_low = X_MIN_ANGLE; // Lower bound of the x-axis search range in degrees
int x_high = X_MAX_ANGLE; // Upper bound of the x-axis search range in degrees
int x_mid = 0; // Middle point of the x-axis search range in degrees
int y_low = Y_MIN_ANGLE; // Lower bound of the y-axis search range in degrees
int y_high = Y_MAX_ANGLE; // Upper bound of the y-axis search range in degrees
int y_mid = 0; // Middle point of the y-axis search range in degrees

// Define a flag to indicate if the tracker is working or not
bool tracker_status = true; // True if working, false if not

void setup() {
  // Set the motor pins as outputs
  pinMode(MOTOR_X_DIR, OUTPUT);
  pinMode(MOTOR_X_STEP, OUTPUT);
  pinMode(MOTOR_Y_DIR, OUTPUT);
  pinMode(MOTOR_Y_STEP, OUTPUT);

  // Set the endstop pins as inputs with pull-up resistors
  pinMode(ENDSTOP_X, INPUT_PULLUP);
  pinMode(ENDSTOP_Y, INPUT_PULLUP);

  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Home the axes by moving them until they hit the endstops
  home_axes();
}

void loop() {
  
    if (tracker_status) {
      // Read and update the values of the photoresistors
      read_ldrs();

      // Perform a binary search to find the sun's position on both axes
      
      find_sun_x(); 
      find_sun_y();

      // Send the current position of the sun over serial port in JSON format
      
      send_position();
    }

    else {
      // Send a failure message over serial port in JSON format
      
      send_failure();
    }

    // Wait for five minutes before repeating
    
    delay(300000);
  
}

// A function to move one step on the x-axis in a given direction

void step_x(int dir) {
  
    if (dir == -1) {
      digitalWrite(MOTOR_X_DIR, LOW); // Set direction to counterclockwise
      
    }
    
    else {
      digitalWrite(MOTOR_X_DIR, HIGH); // Set direction to clockwise
      
    }
    
    digitalWrite(MOTOR_X_STEP, HIGH); // Make a step
    
    delay(STEP_DELAY); 
    
    digitalWrite(MOTOR_X_STEP, LOW); 
    
    delay(STEP_DELAY);
  
}

// A function to move one step on the y-axis in a given direction

void step_y(int dir) {
  
    if (dir == -1) {
      digitalWrite(MOTOR_Y_DIR, LOW); // Set direction to counterclockwise
      
    }
    
    else {
      digitalWrite(MOTOR_Y_DIR, HIGH); // Set direction to clockwise
      
    }
    
    digitalWrite(MOTOR_Y_STEP, HIGH); // Make a step
    
    delay(STEP_DELAY); 
    
    digitalWrite(MOTOR_Y_STEP, LOW); 
    
    delay(STEP_DELAY);
  
}

// A function to home both axes by moving them until they hit the endstops

void home_axes() {
  
    while (digitalRead(ENDSTOP_X) == HIGH) { 
      
        step_x(-1); 
        
        x_pos--; 
        
        if (x_pos < X_MIN_ANGLE) { 
          
            tracker_status = false; 
            
            break;
          
        }
      
    }
    
    while (digitalRead(ENDSTOP_Y) == HIGH) { 
      
        step_y(-1); 
        
        y_pos--; 
        
        if (y_pos < Y_MIN_ANGLE) { 
          
            tracker_status = false; 
            
            break;
          
        }
      
    }
  
}

// A function to read and update the values of the photoresistors

void read_ldrs() {
  
    ldr_top = analogRead(LDR_TOP); 
    
    ldr_bottom = analogRead(LDR_BOTTOM); 
    
    ldr_left = analogRead(LDR_LEFT); 
    
    ldr_right = analogRead(LDR_RIGHT);
  
}

// A function to perform a binary search to find the sun's position on the x-axis

void find_sun_x() {
  
    while (x_low <= x_high) { 
      
        x_mid = (x_low + x_high) / 2; 
        
        move_to_x(x_mid); 
        
        read_ldrs(); 
        
        if (ldr_left > ldr_right) { 
          
            x_high = x_mid - 1;
          
        }
        
        else if (ldr_left < ldr_right) { 
          
            x_low = x_mid + 1;
          
        }
        
        else { 
          
            break;
          
        }
      
    }
  
}

// A function to perform a binary search to find the sun's position on the y-axis

void find_sun_y() {
  
    while (y_low <= y_high) { 
      
        y_mid = (y_low + y_high) / 2; 
        
        move_to_y(y_mid); 
        
        read_ldrs(); 
        
        if (ldr_top > ldr_bottom) { 
          
            y_high = y_mid - 1;
          
        }
        
        else if (ldr_top < ldr_bottom) { 
          
            y_low = y_mid + 1;
          
        }
        
        else { 
          
            break;
          
        }
      
    }
  
}

// A function to move to a given angle on the x-axis

void move_to_x(int angle) {
  
    int steps = angle - x_pos; 
    
    int dir = steps > 0 ? 1 : -1; 
    
    steps = abs(steps); 
    
    for (int i = 0; i < steps; i++) { 
      
        step_x(dir); 
        
        x_pos += dir;
      
    }
  
}

// A function to move to a given angle on the y-axis

void move_to_y(int angle) {
  
    int steps = angle - y_pos; 
    
    int dir = steps > 0 ? 1 : -1; 
    
    steps = abs(steps); 
    
    for (int i = 0; i < steps; i++) { 
      
        step_y(dir); 
        
        y_pos += dir;
      
    }
  
}

// A function to send the current position of the sun over serial port in JSON format

void send_position() {
  
   Serial.print("{\"x\":"); 
   
   Serial.print(x_pos); 
   
   Serial.print(",\"y\":"); 
   
   Serial.print(y_pos); 
   
   Serial.println("}");
  
}

// A function to send a failure message over serial port in JSON format

void send_failure() {
  
   Serial.println("{\"error\":\"Tracker failed\"}");
  
}
