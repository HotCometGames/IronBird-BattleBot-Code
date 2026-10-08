/*
Ok update for 2025 electronics***************************************************
Order of operations:
Choose an ESP32:
If Narrow Pin, use the Arduino Board ESP-32 DEV KIT
If Wide Pin (30 Pin), use the Arduino Board ESP-32 WROOM
If large breakout board, use ESP32-WROOM-DA-MODULE


1) Get the Bluetooth MAC Address of the ESP by running the BLE MAC Address Code. Upload the code, open serial read, and reboot ESP. MAC Address should display. Copy this, its used for everything



2) Open the specific code for your robot. Make sure you have proper PS4 Libraries. Paste the ESP MAC Address into the MAC Address part of the code. Make sure, in the Arduino software, you have the "Erase All Flash Before Upload" enabled. This clears any memory of previous handshaking with other controllers.



3) Plug in PS4 Controller, run SIXAXISPAIR TOOL, and paste the same MAC Address into the controller. The controller will only store one at a time, and will be looking for this address (its the ESP's address your using)

*/



// ALL CHANGES MADE BY CALEB GLEASON WILL BE MARKED BY A "!!" IN COMMENTS



#include <PS4Controller.h>  // Include the PS4 controller library
#include <ESP32Servo.h>     // Include the ESP32 servo library




Servo leftServo;  // Create a servo object for the first servo        !! Changed name from myservo1 for clarification
Servo rightServo;  // Create a servo object for the second servo      !! Changed name from myservo2 for clarification
Servo weaponServo;  // Create a servo object for the third servo      !! Changed name from myservo3 for clarification




int LEFTjoystick = 17;  // Define the GPIO pin for the left joystick position   !! Changed name from LEFTjoystickPos for clarification and because variable w same name as initialized later
int RIGHTjoystick = 19; // Define the GPIO pin for the right joystick position  !! Changed name from RIGHTjoystickPos for clarification and because variable w same name as initialized later
int Weapon = 26;           // Define the GPIO pin for the weapon servo




bool inverseControls = false;
bool motorsConnected = false;                 // !! Added to detect if motors are on or off
bool upButtonPreviouslyPressed = false;       // !! Added to make inverse control toggling safer




void setup() {
  pinMode(2, OUTPUT);     // Set GPIO pin 2 as an output
  digitalWrite(2, HIGH);   // Set GPIO pin 2 to LOW (turn off LED)




  pinMode(17, OUTPUT);    // Set GPIO pin 17 as an output
  digitalWrite(17, LOW);  // Set GPIO pin 17 to LOW




  pinMode(19, OUTPUT);    // Set GPIO pin 19 as an output
  digitalWrite(19, LOW);  // Set GPIO pin 19 to LOW




  pinMode(26, OUTPUT);     // Set GPIO pin 26 as an output
  digitalWrite(26, LOW);   // Set GPIO pin 26 to LOW




  Serial.begin(115200);   // Initialize serial communication at 115200 baud rate
  PS4.begin("a0:b7:65:58:80:4e");  // Initialize PS4 controller with MAC address
  Serial.println("Initialize.");   // Print initialization message




  leftServo.setPeriodHertz(50); // Set the servo update rate to 50 Hz for servo1
  rightServo.setPeriodHertz(50); // Set the servo update rate to 50 Hz for servo2
  weaponServo.setPeriodHertz(50); // Set the servo update rate to 50 Hz for servo3


  // Attach the servos to their respective pins with min and max pulse widths
  leftServo.attach(LEFTjoystick, 1000, 2000);
  rightServo.attach(RIGHTjoystick, 1000, 2000);
  weaponServo.attach(Weapon, 1000, 2000);
  motorsConnected = true; // !! state the motors turned on
}




void loop() {
  if (PS4.isConnected()) {  // Check if PS4 controller is connected




    //if (PS4.L1()) {  // If L1 button is pressed
      //digitalWrite(2, HIGH); // Turn on LED connected to GPIO pin 2
    //} else {  // If L1 button is not pressed
      //digitalWrite(2, LOW);  // Turn off LED connected to GPIO pin 2
    //}
    // If L1 is pressed, the blue LED on the ESP (set with Pin2) will illuminate.








    if(!motorsConnected)  // !!
    {                     // !!
      connectAllServos(); // !! added to connect motors if they were not on while PS4 controller is connected
    }                     // !!
  } else {
    if (motorsConnected)     // !!
    {                        // !!
      disconnectAllServos(); // !! added to disconnect all motors if PS4 controller happens to disconnect
    }                        // !!
  }




  // Inverse Controls
  if (PS4.Up()) {
    if(!upButtonPreviouslyPressed)          // !! make it so when you hold down the button, it doesnt keep spamming switching the types of controls. One press and holding it down will invert it, lifting up and pressing it again ill invert it again
    {                                       // !!
      inverseControls = !inverseControls;   // !! quicker way of previous code, or making it equal to opposite of itself
      upButtonPreviouslyPressed = true;     // !! Sets bool true, to make the if statement work
    }                                       // !!
  } else {                                  // !!
    upButtonPreviouslyPressed = false;      // !! allows you to be able to invert again after release of button
  }




  int LEFTjoystickVal = map(PS4.LStickY(), -128, 128, 10, 255);  // !! Changed name because it was unclear and interfered with a previous variable of the same name
  int RIGHTjoystickVal = map(PS4.RStickY(), -128, 128, 10, 255); // !! Changed name because it was unclear and interfered with a previous variable of the same name




  if (!inverseControls) {
    leftServo.write(LEFTjoystickVal);
    rightServo.write(RIGHTjoystickVal);
  } else {
    leftServo.write(255 - LEFTjoystickVal);
    rightServo.write(255 - RIGHTjoystickVal);
  }




  // Control the weapon servo based on L2 and R2 button presses
  if (PS4.L2() && PS4.R2()) {
    weaponServo.write(132);  // neutral when both L2 and R2 are pressed          !! Changed val to 132 because it seems like thats what netural really is
  } else if (PS4.L2()) {
    weaponServo.write(0);  // Max speed backwards when only L2 is pressed
  } else if (PS4.R2()) {
    weaponServo.write(255);  // Max speed forward when only R2 is pressed
  } else {
    weaponServo.write(132);  // Default to 132 when neither L2 nor R2 is pressed  !! Changed val to 132 because it seems like thats what netural really is
  }
}




// !! This function was made to disconnect all of the motors
void disconnectAllServos() {
  weaponServo.write(132);
  weaponServo.detach();
  digitalWrite(Weapon, LOW);




  leftServo.write(132);
  leftServo.detach();
  digitalWrite(LEFTjoystick, LOW);




  rightServo.write(132);
  rightServo.detach();
  digitalWrite(RIGHTjoystick, LOW);




  motorsConnected = false;
}




// !! This function was made to connect all of the motors
void connectAllServos()
{
  weaponServo.attach(Weapon, 1000, 2000);
  weaponServo.write(132);




  leftServo.attach(LEFTjoystick, 1000, 2000);
  leftServo.write(132);




  rightServo.attach(RIGHTjoystick, 1000, 2000);
  rightServo.write(132);




  motorsConnected = true;
}













