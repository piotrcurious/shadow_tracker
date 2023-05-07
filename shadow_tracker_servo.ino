
//Servo motor library
#include <Servo.h>

//Initialize variables
int ldrtopr = 0; // top-right LDR
int ldrtopl = 1; // top-left LDR
int ldrbotr = 2; // bottom-right LDR
int ldrbotl = 3; // bottom-left LDR

//Declare two servos
Servo servo_updown;
Servo servo_rightleft;

int threshold_value = 10; //measurement sensitivity

void setup() {
  //Attach servos to pins
  servo_updown.attach(5); //Servo motor up-down movement
  servo_rightleft.attach(6); //Servo motor right-left movement
}

void loop() {
  //capturing analog values of each LDR
  int topr = analogRead(ldrtopr); //capturing analog value of top right LDR
  int topl = analogRead(ldrtopl); //capturing analog value of top left LDR
  int botr = analogRead(ldrbotr); //capturing analog value of bot right LDR
  int botl = analogRead(ldrbotl); //capturing analog value of bot left LDR

  // calculating average
  int avgtop = (topr + topl) / 2; //average of top LDRs
  int avgbot = (botr + botl) / 2; //average of bottom LDRs
  int avgleft = (topl + botl) / 2; //average of left LDRs
  int avgright = (topr + botr) / 2; //average of right LDRs

  //Get the difference
  int diffelev = avgtop - avgbot; //Get the difference between top and bottom LDRs
  int diffazi = avgright - avgleft; //Get the difference between right and left LDRs

  //left-right movement of solar tracker
  if (abs(diffazi) >= threshold_value) { //Change position only if light difference is bigger than the threshold_value
    if (diffazi > 0) {
      if (servo_rightleft.read() < 180) {
        servo_rightleft.write((servo_rightleft.read() + 2)); //Move right by increasing the angle by 2 degrees
      }
    }
    if (diffazi < 0) {
      if (servo_rightleft.read() > 0) {
        servo_rightleft.write((servo_rightleft.read() - 2)); //Move left by decreasing the angle by 2 degrees
      }
    }
  }

  //up-down movement of solar tracker
  if (abs(diffelev) >= threshold_value) { //Change position only if light difference is bigger than the threshold_value
    if (diffelev > 0) {
      if (servo_updown.read() < 180) {
        servo_updown.write((servo_updown.read() - 2)); //Move up by decreasing the angle by 2 degrees
      }
    }
    if (diffelev < 0) {
      if (servo_updown.read() > 0) {
        servo_updown.write((servo_updown.read() + 2)); //Move down by increasing the angle by 2 degrees
      }
    }
  }
}


//Source: Conversation with Bing, 5/7/2023
//(1) Building an Automatic Solar Tracker With Arduino UNO. https://www.instructables.com/Building-an-Automatic-Solar-Tracker-With-Arduino-U/.
//(2) Arduino Solar Tracker | Arduino Project Hub. https://projecthub.arduino.cc/Aboubakr_Elhammoumi/77347b69-2ade-4a44-b724-3bb91e954188.
//(3) Arduino Solar Tracker (Single or Dual Axis) - Instructables. https://www.instructables.com/Arduino-Solar-Tracker-Single-or-Dual-Axis/.
