// // Include the AccelStepper Library
//#include <AccelStepper.h>
//
//// ressources: https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#ace236ede35f87c63d18da25810ec9736 
//
//// Define pin connections
//const int dirPinX = 4;
//const int stepPinX = 5;
//const int dirPinY = 6;
//const int stepPinY = 7;
//
//// Define motor interface type
//#define motorInterfaceType 1 //1 means a stepper driver
//
//// Creates an instance
//AccelStepper myStepperX(motorInterfaceType, stepPinX, dirPinX);
//AccelStepper myStepperY(motorInterfaceType, stepPinY, dirPinY);
//
//void setup() {
// // set the maximum speed, acceleration factor,
//  // initial speed and the target position
//  myStepperX.moveTo(100);
//  myStepperX.setMaxSpeed(1000);
////  myStepperY.setMaxSpeed(200);
////  myStepperY.setAcceleration(200);
////  myStepperY.setSpeed(200);
////  myStepperY.move(100);
//}
//
//void loop() {
// myStepperX.setSpeed(1000);
// myStepperX.setAcceleration(200);
// 
//  while(myStepperX.distanceToGo() != 0){
//    myStepperX.run();
//  }
//
//  myStepperX.moveTo(-100);
////  myStepperX.stop();
//  delay(2000);

//  while(myStepperY.distanceToGo() != 0){
//    myStepperY.runSpeed();
//  }
//
//  myStepperY.move(-myStepperY.currentPosition());
//  myStepperY.stop(); 
//}



//// Include the AccelStepper Library
//#include <AccelStepper.h>
//
//// Define pin connections
//const int dirPinX = 4;
//const int stepPinX = 5;
//const int dirPinY = 6;
//const int stepPinY = 7;
//
//// Define motor interface type
//#define motorInterfaceType 1
//
//// Creates an instance
//AccelStepper myStepperX(motorInterfaceType, stepPinX, dirPinX);
//AccelStepper myStepperY(motorInterfaceType, stepPinY, dirPinY);
//
//void setup() {
//  // set the maximum speed, acceleration factor,
//  // initial speed and the target position
//  myStepperX.setMaxSpeed(200);
//  myStepperX.setAcceleration(200);
//  myStepperX.setSpeed(200);
//  myStepperX.moveTo(100);
//  myStepperY.setMaxSpeed(200);
//  myStepperY.setAcceleration(200);
//  myStepperY.setSpeed(200);
//  myStepperY.moveTo(100);
//}
//
//void loop() {
//  // Change direction once the motor reaches target position
//  if (myStepperX.distanceToGo() == 0) 
//    myStepperX.moveTo(-myStepperX.currentPosition());
//
//  if (myStepperY.distanceToGo() == 0) 
//    myStepperY.moveTo(-myStepperY.currentPosition());
//
//  // Move the motor one step
//  myStepperX.run();
//  myStepperY.run();
//}

#include <AccelStepper.h>
// Define pin connections
const int dirPinX = 2;
const int stepPinX = 3;
//const int dirPinY = 6;
//const int stepPinY = 7;

//// Define motor interface type
#define motorInterfaceType 1 //1 means a stepper driver
AccelStepper stepperX(motorInterfaceType, stepPinX, dirPinX);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//AccelStepper stepperY(motorInterfaceType, stepPinY, dirPinY);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

int flag = 1;

void setup()
{  
  Serial.begin(9600);
  stepperX.setMaxSpeed(2000);
  stepperX.setAcceleration(2000);
  stepperX.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1
//  stepperY.setMaxSpeed(2000);
//  stepperY.setAcceleration(2000);
//  stepperY.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1

  //stepperY.setMaxSpeed(150);
  //stepperY.setAcceleration(300);
}
void loop() {  

  stepperX.moveTo(1400);
  stepperX.setSpeed(1000);
//  stepperY.moveTo(200);
//  stepperY.setSpeed(100);
  while (stepperX.distanceToGo() != 0){ // Full speed up to 300
    stepperX.runSpeedToPosition(); 
//    stepperY.runSpeedToPosition(); 
  }

//  delay(1000);

  stepperX.moveTo(0);
  stepperX.setSpeed(1000);
//  stepperY.moveTo(0);
//  stepperY.setSpeed(100);
  while (stepperX.distanceToGo() != 0){ // Full speed up to 300
    stepperX.runSpeedToPosition(); //this function is not blocking, it needs to be run every time to make it move
//    stepperY.runSpeedToPosition();  
  }

//  delay(1000);
  
  
//  stepperY.moveTo(200);
//  while (stepperY.currentPosition() != 200) // Full speed basck to 0
//    stepperY.run();
//  stepperY.stop(); // Stop as fast as possible: sets new target
//  stepperY.runToPosition(); 
//  //delay(3000);
//  // Now stopped after quickstop

//  stepperX.moveTo(-200);
//  stepperX.setSpeed(100);
//  while (stepperX.distanceToGo() != 0){ // Full speed up to 300
//    stepperX.runSpeedToPosition(); 
//  }
//  stepperX.stop(); // Stop as fast as possible: sets new target
//  stepperX.runToPosition();
//  // Now stopped after quickstop
//  // Now go backwards
//  delay(1000);

//  stepperY.moveTo(-200);
//  while (stepperY.currentPosition() != 0) // Full speed basck to 0
//    stepperY.run();
//  stepperY.stop(); // Stop as fast as possible: sets new target
//  stepperY.runToPosition(); 
//  // Now stopped after quickstop
 
}


//#include <AccelStepper.h>
//// Define a stepper and the pins it will use
//const int dirPinX = 6;
//const int stepPinX = 7;
//
////// Define motor interface type
//#define motorInterfaceType 1 //1 means a stepper driver
//AccelStepper stepper(motorInterfaceType, stepPinX, dirPinX);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
//void setup()
//{  
//  // Change these to suit your stepper if you want
//  stepper.setMaxSpeed(100);
//  stepper.setAcceleration(200);
//  stepper.moveTo(200);
//}
//void loop()
//{
//    // If at the end of travel go to the other end
//    if (stepper.distanceToGo() == 0)
//      stepper.moveTo(-stepper.currentPosition());
//    stepper.run();
//}
