#include <SPI.h>
#include <SD.h>
#include <AccelStepper.h>

#define SD_ChipSelectPin A0 //Chip select is pin A0

File myFile;

// Define pin connections
const int dirPinExt = 2;
const int stepPinExt = 3;
const int dirPinX = 4;
const int stepPinX = 5;
const int dirPinY = 6;
const int stepPinY = 7;
const int dirPinZ = 8;
const int stepPinZ = 9;
const int zMotorSwitch = A1;

//// Define motor interface type
#define motorInterfaceType 1 //1 means a stepper driver
AccelStepper stepperExt(motorInterfaceType, stepPinExt, dirPinExt);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepperX(motorInterfaceType, stepPinX, dirPinX);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepperY(motorInterfaceType, stepPinY, dirPinY);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepperZ(motorInterfaceType, stepPinZ, dirPinZ);; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

//Printer settings  ////////////////////////////////////////////////////////////////////////

float pulley_radius = 6; //in mm
float step_precision = 0.0314159/4; //in rad/step. 1.8deg/4 = 0.45deg
const int MotorAccel = 1500;
const int xySpeedMax = 1500; //in steps/s. Equals a max speed of 71mm/s
const int zSpeedMax = 1500;
const int extSpeedMax = 1400; //60mm/s
const int steps_per_rot = 800; //800 steps per 1 rotation, in quarter mode 
const int thread_lead = 1.43; // 1.43mm of travel for 1 rotation
const int temp_high_limit = 270; //on a scale of 0-1023
const int temp_low_limit = 230; //on a scale of 0-1023

//End of printer settings ////////////////////////

//Start of Variables Section /////////////////////////////////////////////////////////////// 

//readLine()and parseLine() variables
const int maxLength = 150; 
char inputData[maxLength];
char currentChar;
int index;

const int axisLength = 12;
float xLast = 0.0;
float yLast = 0.0;
float zLast = 0.0;
float extLast = 0.0;
float feedLast = 0.0;
float x;
float y;
float z;
float ext;
float feed;
float dX;
float dY;
float dZ;
float dFeed;
float dExt;
int xSteps;
int ySteps;
int zSteps;
int extSteps;
char xChar[axisLength];
char yChar[axisLength];
char zChar[axisLength];
char extChar[axisLength];
char feedChar[axisLength];
int xEmpty = 1;
int yEmpty = 1;
int zEmpty = 1;
int extEmpty = 1;
int feedEmpty = 1;

float tf;
float Vx;
float Vy;
float Vext;


//Heater Variables
int heaterSwitch = 10;
int heaterTemp = A2;
float temp = 0.0;

//End of variables Section ///////////////////////////

//Start of Functions Section ////////////////////////////////////////////////////////////////

void readLine(){
  resetData();
  index = 0;
  do{ //read the txt file line 
    currentChar = myFile.read();
    inputData[index] = currentChar;
//    Serial.print(inputData[index]);
    index = index + 1;
  } while(currentChar != '\n');
  
  return;
}

void parseLine(){
  index = 0;
  if(inputData[index] == 'G'){ //G section
    index = index + 1;      
    if(inputData[index] == '0' || inputData[index] == '1'){ //G0 and G1 section
      index = index + 2; //skip the first space after G0/1 and go to the first letter
      G01_Command();
    }
    else if(inputData[index] == '2'){ //clockwise circle section
     //not needed 
    }
    else if(inputData[index] == '3'){ //counter-clockwise circle section
     //not needed
    }
    else if(inputData[index] == '9' && inputData[index + 1] == '2' ){ //Set position section
      setNewCoordinate();
    }
    else {
      //ignore the other G-code commands
    }
  }
  else if(inputData[index] =='M'){ //M section
    index = index + 1;
    if(inputData[index] == '1' && inputData[index + 1] == '0' && inputData[index + 2] == '4' ){ //Set Hotend Temperature version 1 section
//      setHotEnd();     
    }
    else if(inputData[index] == '1' && inputData[index + 1] == '0' && inputData[index + 2] == '5' ){ //Record Temperature section
      //not needed
    }
    else if(inputData[index] == '1' && inputData[index + 1] == '0' && inputData[index + 2] == '9' ){ //Set Hotend Temperature version 2 section
//      setHotEnd();    
    }
    else {
      //ignore the other G-code commands
    }
  }
  else { //else it's a comment section or the command isn't necessary for the design
    //do nothing
  }
    
  return;
}

void resetData(){
  int i = 0;
  while(i<maxLength){
    inputData[i] = '\0'; // \0 means an empty array index, this will be used to find the end of the parsed command
    i = i + 1; 
  }
  i = 0;
  while(i<axisLength){
    xChar[i] = '\0'; // \0 means an empty array index, this will be used to find the end of the parsed command
    yChar[i] = '\0';
    zChar[i] = '\0';
    extChar[i] = '\0';
    feedChar[i] = '\0';
    i = i + 1; 
  }
}

void setHotEnd() {
  digitalWrite(heaterSwitch, HIGH);
  temp = analogRead(heaterTemp);
  while(temp < temp_high_limit){
    Serial.println(temp);
    delay(1500);
    temp = analogRead(heaterTemp);
  }
  Serial.println(F("Done heating"));
  digitalWrite(heaterSwitch, LOW);
}

void heater_check(){
  temp = analogRead(heaterTemp);
  if(temp < temp_low_limit){
    digitalWrite(heaterSwitch, HIGH);
    Serial.println(F("on"));
    Serial.println(temp);
  }
  else if(temp > temp_high_limit){
    digitalWrite(heaterSwitch, LOW);
    Serial.println(F("off"));
    Serial.println(temp);
  }
} 

void setNewCoordinate() {
  if(xEmpty == 0){
    //mm*(steps in 1 rot/mm in 1 rot)
    stepperX.setCurrentPosition((xLast*steps_per_rot/(2*3.14*pulley_radius))+1); //position (in steps) that will become the new zero location
    xLast = 0;
  }
  if(yEmpty == 0){
    stepperY.setCurrentPosition((yLast*steps_per_rot/(2*3.14*pulley_radius))+1); // +1 so that current position is 1 instead of 0 if xLast = 0. 0 doesn't work for the function
    yLast = 0;
  }
  if(zEmpty == 0){
    stepperZ.setCurrentPosition((zLast*steps_per_rot/(2*3.14*pulley_radius))+1); 
    zLast = 0;
  }
  if(extEmpty == 0){
    stepperExt.setCurrentPosition((extLast*steps_per_rot/(2*3.14*pulley_radius))+1); 
    extLast = 0;
  }
}

void extractCommandData(){
  int i;
  while(inputData[index] != '\0'){
    //X section
    if(inputData[index] == 'X'){
      xEmpty = 0;
      i = 0;
      index = index + 1;
      while(inputData[index] != ' ' && inputData[index] != '\0'){
        xChar[i] = inputData[index];
        index = index + 1;
        i = i + 1;
      }
      index = index + 1; //skip space 
      x = atof(xChar); //turn the strings into float values 
    }
    
    //Y section
    else if(inputData[index] == 'Y'){
      yEmpty = 0;
      i = 0;
      index = index + 1;
      while(inputData[index] != ' ' && inputData[index] != '\0'){
        yChar[i] = inputData[index];
        index = index + 1;
        i = i + 1;
      }
      index = index + 1; //skip space
      y = atof(yChar);
    }

    //Z section
    else if(inputData[index] == 'Z'){
      zEmpty = 0;
      i = 0;
      index = index + 1;
      while(inputData[index] != ' ' && inputData[index] != '\0'){
        zChar[i] = inputData[index];
        index = index + 1;
        i = i + 1;
      }
      index = index + 1; //skip space
      z = atof(zChar);
    }

    //ext section
    else if(inputData[index] == 'E'){
      extEmpty = 0;
      i = 0;
      index = index + 1;
      while(inputData[index] != ' ' && inputData[index] != '\0'){
        extChar[i] = inputData[index];
        index = index + 1;
        i = i + 1;
      }
      index = index + 1; //skip space
      ext = atof(extChar);
    }

    //feed section
    else if(inputData[index] == 'F'){
      feedEmpty = 0;
      i = 0;
      index = index + 1;
      while(inputData[index] != ' ' && inputData[index] != '\0'){
        feedChar[i] = inputData[index];
        index = index + 1;
        i = i + 1;
      }
      index = index + 1; //skip space
      feed = atof(feedChar);
    }

    else {
      index = index + 1;
    }
  }
}

void G01_Command() {
  xEmpty = 1; //1 means true, i.e there is no x value in the command 
  yEmpty = 1;
  zEmpty = 1;
  extEmpty = 1;
  feedEmpty = 1;
  extractCommandData();
  //find the number of steps that each motor requires
  if(xEmpty == 1){
    x = xLast; //dX = 0
  }
   if(yEmpty == 1){
    y = yLast; 
  }
   if(zEmpty == 1){
    z = zLast; 
  } 
   if(extEmpty == 1){
    ext = extLast; 
  }
   if(feedEmpty == 1){
    feed = feedLast*60; 
  }
  
  dX = (x-xLast); 
  dY = (y-yLast);
  dZ = (z-zLast);
  dExt = (ext-extLast);

  xSteps = round(x/(pulley_radius*step_precision))+1; //in absolute mode, so using x and not dX
  ySteps = round(y/(pulley_radius*step_precision))+1;
  zSteps = round(z*(1/thread_lead)*steps_per_rot)+1;
  extSteps = round(ext/(pulley_radius*step_precision))+1;

  //calculate the max speed for the motors
  feed = feed/60.0; //convert to mm/sec, the G-code is in mm/min
  if(xEmpty == 0 || yEmpty == 0 || extEmpty == 0){ //if there are movements in the x/y axis
    dFeed = sqrt(pow(dX,2)+pow(dY,2));
    tf = dFeed/feed;
    Vx = dX*feed/dFeed;
    Vy = dY*feed/dFeed;
    Vx = abs(Vx*steps_per_rot/(2*3.14159*pulley_radius)); //convert to steps/s by doing mm/s * 800steps/arc length in mm
    Vy = abs(Vy*steps_per_rot/(2*3.14159*pulley_radius));
    
    if(extEmpty == 0){ //if there is movement in the ext axis
      if(dFeed == 0){
        Vext = abs(feed*steps_per_rot/(2*3.14159*pulley_radius));
      }
      else {
//        Vext = abs((dFeed/tf)*steps_per_rot/(2*3.14159*pulley_radius));
        Vext = abs(feed*steps_per_rot/(2*3.14159*pulley_radius));
      }
    }
    
//    Serial.println(feed);
//    Serial.println(xSteps);
//    Serial.println(ySteps);
//    Serial.println(Vx);
//    Serial.println(Vy); 
    runXYExtMotors();
  }

  if(zEmpty == 0){ //if there is movement in the z axis
    digitalWrite(zMotorSwitch, LOW);
    runZMotor();
    digitalWrite(zMotorSwitch, HIGH);
  }

  xLast = x;
  yLast = y;
  zLast = z;
  extLast = ext;
  feedLast = feed;
}

void runXYExtMotors() {
  stepperX.moveTo(xSteps);
  stepperX.setSpeed(Vx);
  stepperY.moveTo(ySteps);
  stepperY.setSpeed(Vy);
  
  if(extEmpty == 0){
    stepperExt.moveTo(extSteps); 
    stepperExt.setSpeed(-Vext); //because the motor is reversed
  }
  
  if(xEmpty == 0 && yEmpty == 0 && extEmpty == 0){ //x y ext
    while (stepperX.distanceToGo() != 0){ 
      stepperX.runSpeedToPosition(); 
      stepperY.runSpeedToPosition(); 
      stepperExt.runSpeed(); 
    }
  }

  else if(xEmpty == 1 && yEmpty == 0 && extEmpty  == 0){ //y ext
    while (stepperY.distanceToGo() != 0){ 
      stepperY.runSpeedToPosition();
      stepperExt.runSpeed(); 
    }
  }

  else if(xEmpty == 0 && yEmpty == 1 && extEmpty  == 0){ //x ext  
    while (stepperX.distanceToGo() != 0){ 
      stepperX.runSpeedToPosition();
      stepperExt.runSpeed(); 
    }
  }

  else if(xEmpty == 0 && yEmpty == 0 && extEmpty  == 1){ //x y  
    while (stepperX.distanceToGo() != 0){ 
      stepperX.runSpeedToPosition();
      stepperY.runSpeedToPosition(); 
    }
  }

  else{ //ext
    while (stepperExt.distanceToGo() != 0){ 
      stepperExt.runSpeedToPosition();
    }
  }
  
}

void runZMotor() {
  stepperZ.moveTo(zSteps);
  stepperZ.setSpeed(zSpeedMax);
  while (stepperZ.distanceToGo() != 0){ 
    stepperZ.runSpeedToPosition(); 
  }
}

//End of Functions Section /////////////////////////

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }
  Serial.println(F("initialization done."));
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  resetData();

  stepperExt.setMaxSpeed(extSpeedMax);
  stepperExt.setAcceleration(MotorAccel);
  stepperExt.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1
  stepperX.setMaxSpeed(xySpeedMax);
  stepperX.setAcceleration(MotorAccel);
  stepperX.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1
  stepperY.setMaxSpeed(xySpeedMax);
  stepperY.setAcceleration(MotorAccel);
  stepperY.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1
  stepperZ.setMaxSpeed(zSpeedMax);
  stepperZ.setAcceleration(MotorAccel);
  stepperZ.setCurrentPosition(1); //need this to make the motor start moving. Keep it at value of 1

  pinMode(zMotorSwitch, OUTPUT);
  digitalWrite(zMotorSwitch, HIGH);
  pinMode(heaterSwitch, OUTPUT);
  digitalWrite(heaterSwitch, LOW);
  delay(3000);
  
}
void loop() {
  myFile = SD.open("test.txt");
  
  if (myFile) {
    while(myFile.available()){
      readLine();
      parseLine();
//      heater_check();
    }
    
    // close the file:
    myFile.close();
    Serial.println(F("Print is done. Please reposition the axis for next print."));
  } 
}
