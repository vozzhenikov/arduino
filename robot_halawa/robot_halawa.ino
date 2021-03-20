/*
Author: Muhammed Azhar
visit - robotechmaker.com
*/

//#include <AFMotor.h>
#include <L298P.h>
#include <SoftwareSerial.h> 

#include <NewPing.h>

// L298P Motor shiled pin map
#define LEFT_MOTOR_DIRECTION  13
#define RIGHT_MOTOR_DIRECTION 12
#define RIGHT_MOTOR_PWM       10
#define LEFT_MOTOR_PWM        11
#define PAN_SERVO_PIN         9
#define ECHO_PIN              8
#define TRIGGER_PIN           7
#define BUZZER                4
#define TILT_SERVO_PIN        6
// #define LEFT_MOTOR_IR_SENSOR_PIN 2
// #define RIGHT_MOTOR_IR_SENSOR_PIN 3

// Custom pin map
#define BT_RX                 2
#define BT_TX                 3 //3.5v

// Algorythm params
#define MAX_DISTANCE 200

#define FORWARD      L298P::Direction::FORWARD
#define BACKWARD     L298P::Direction::BACKWARD

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
SoftwareSerial BtSerial(BT_RX, BT_TX);

L298P motorRight(RIGHT_MOTOR_DIRECTION, RIGHT_MOTOR_PWM, true);
L298P motorLeft(LEFT_MOTOR_DIRECTION, LEFT_MOTOR_PWM);

//Constants and variable
char dataIn = 'S';
long distance;
int vel = 255; //Max velocity
char command = 'S';
char lastCommand;
int mode;
int lastMode;


char *modeNames[] = {"Manual", "Avoid obstacle ", "Wall follower"};

void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps
  BtSerial.begin(9600);
  
  pinMode(TRIGGER_PIN, OUTPUT);// set the trig pin to output (Send sound waves)
  pinMode(ECHO_PIN, INPUT);// set the echo pin to input (recieve sound waves)  

  //Initalization messages
  //log_info(" Reday to go!");
  BtSerial.println("  Reday to go!");
  
  //turn off motors
  motorRight.stop();
  motorLeft.stop();
}

void loop() {

  
  command = readCommand();
  if (isCommandChanged(lastCommand, command)) {
    mode = defineMode(command);
    if (isModeChanged(lastMode, mode)) {
      //log_info ("  Mode activated: "+ modeNames[mode]);
      BtSerial.print("  Mode activated: ");
      BtSerial.println(modeNames[mode]);
      Serial.println(modeNames[mode]);
      lastMode = mode;
    }
  }

  switch (mode) {
    case 0:
      manualStep(command);
    break;
    case 1: // avoid obstacle 
      avoidStep(command);
    break;
    case 2: // wall follower
      followStep(command);
    break;
  }

  lastCommand = command;
}

int defineMode(char p_command) {
  switch (p_command) {
    case 'a': // avoid obstacle 
      return 1; 
    case 'w':
      return 2; // wall follower
    default:    // manual
      return 0; 
  }
}

bool isModeChanged (int p_last_mode, int p_mode) {
  return p_mode != p_last_mode;
}

bool isCommandChanged (int p_lastCommand, int p_command) {
  return p_command != p_lastCommand;
}

void manualStep(char p_command) {
  switch (p_command){
    case 'F': // F, move forward
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel);
    motorRight.forward();      
    motorLeft.forward();
    Serial.println("Run forward");
    break;
    
    case 'B': // B, move back
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel);
    motorRight.backward();      
    motorLeft.backward();
    Serial.println("Run backward");
    break;
    
    case 'L':// L, move wheels left
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/4);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    break;
    
    case 'R': // R, move wheels right
    motorRight.setSpeed(vel/4);
    motorLeft.setSpeed(vel);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    break;
    
    case 'I': // I, turn right forward
    motorRight.setSpeed(vel/2);
    motorLeft.setSpeed(vel);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    break;
    
    case 'J': // J, turn right back
    motorRight.setSpeed(vel/2);
    motorLeft.setSpeed(vel);  
    motorRight.run(BACKWARD);     
    motorLeft.run(BACKWARD);
    break;
    
    case 'G': // G, turn left forward
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/2);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    break;
    
    case 'H': // H, turn left back
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/2);
    motorRight.run(BACKWARD);     
    motorLeft.run(BACKWARD);
    break;
    
    case 'S': 
   // S, stop
    motorRight.stop();
    motorLeft.stop();
    break;
  }
}



void followStep(char p_command) {
  motorRight.setSpeed(vel); //set the speed of the motors, between 0-255
  motorLeft.setSpeed (vel); 

  distance = sonar.ping_cm();
  //log_debug("distance "+distance);
  
  if (distance < 30) /*The distance that need to to keep with the wall */ {   
    //log_debug ("Wall is ditected. Turning to forllow the wall" );
    
    motorRight.setSpeed(vel);
    motorLeft.stop();
    motorRight.run(FORWARD);
    delay(500); // wait for a second
  } else {
    //log_debug ("No Wall detected. turning round");
    delay (15);
    motorRight.stop();
    motorLeft.setSpeed(vel); 
    motorLeft.run (FORWARD);
  }
}


void avoidStep(char p_command) {
  
  motorRight.setSpeed(vel); //set the speed of the motors, between 0-255
  motorLeft.setSpeed (vel);  
  
  distance = sonar.ping_cm();
  //log_debug("distance "+ distance);
  
  if (distance < 25)/*if there's an obstacle 25 centimers, ahead, do the following: */ {   
    //log_debug ("Close Obstacle detected!" );
    //log_debug ("Obstacle distance: ");
    //log_debug ("Distance From Robot is " + distance + " cm" );

    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel); 
    motorLeft.run(BACKWARD);  // Turn as long as there's an obstacle ahead.
    motorRight.run (FORWARD);
  } else {
    Serial.println ("No obstacle detected. going forward");
    delay (15);
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel);
    motorRight.run(FORWARD); //if there's no obstacle ahead, Go Forward! 
    motorLeft.run(FORWARD);  
  } 
}

/*
void log_debug(char[] text) {
  //log(text);  //debug disabled
}

void log_info(char[] text) {
  log(text);
}

void log(char[] text) {
  Serial.println(text);
  BtSerial.println(text);
} */

//get bluetooth code received from serial port
int readCommand(){
  //if (Serial.available() > 0){// if there is valid data in the serial port
  //dataIn = Serial.read();// stores data into a varialbe

  //char command;

  if (BtSerial.available()) {// if there is valid data in the serial port
    dataIn = BtSerial.read();// stores data into a varialb
    
    //log_debug("Input command: "+dataIn);

    //decode the command
    if (dataIn == 'F'){//Forward
      command = 'F';
    }
    else if (dataIn == 'B'){//Backward
      command = 'B';
    }
    else if (dataIn == 'L'){//Left
      command = 'L';
    }
    else if (dataIn == 'R'){//Right
      command = 'R';
    }
    else if (dataIn == 'I'){//Froward Right
      command = 'I';
    }
    else if (dataIn == 'J'){//Backward Right
      command = 'J';
    }
    else if (dataIn == 'G'){//Forward Left
      command = 'G';
    }    
    else if (dataIn == 'H'){//Backward Left
      command = 'H';
    }
    else if (dataIn == 'S'){//Stop
      command = 'S';
    }
    /*
    else if (dataIn == '0'){//Speed 0
      vel = 0;
    }
    else if (dataIn == '1'){//Speed 25
      vel = 25;
    }
    else if (dataIn == '2'){//Speed 50
      vel = 50;
    }
    else if (dataIn == '3'){//Speed 75
      vel = 75;
    }
    else if (dataIn == '4'){//Speed 100
      vel = 100;
    }
    else if (dataIn == '5'){//Speed 125
      vel = 125;
    }
    else if (dataIn == '6'){//Speed 150
      vel = 150;
    }
    else if (dataIn == '7'){//Speed 175
      vel = 175;
    }
    else if (dataIn == '8'){//Speed 200
      vel = 200;
    }
    else if (dataIn == '9'){//Speed 225
      vel = 225;
    } */   
    else if (dataIn == 'w'){//Extra On
      command = 'w';
    }
    else if (dataIn == 'a'){//Extra On
      command = 'a';
    }

  }

  //log_debug("Decoded command: "+command);
  return command;
}
