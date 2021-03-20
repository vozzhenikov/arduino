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
char determinant;
char det;
char prevDet = 'S';
int vel = 255; //Max velocity

void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps
  BtSerial.begin(9600);
  
  pinMode(TRIGGER_PIN, OUTPUT);// set the trig pin to output (Send sound waves)
  pinMode(ECHO_PIN, INPUT);// set the echo pin to input (recieve sound waves)  

  //Initalization messages
  Serial.println(" Mr.robot");
  Serial.println(" Reday for working!");
  BtSerial.println(" Reday for working!");
  
  //turn off motors
  motorRight.stop();
  motorLeft.stop();
}

void loop() {
  det = readCommand();
  //serial code analysis
  if (det != prevDet){
    prevDet = det;
  switch (det){
    case 'F': // F, move forward
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel);
    motorRight.forward();      
    motorLeft.forward();
    Serial.println("Run forward");
    //det = readCommand();
    break;
    
    case 'B': // B, move back
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel);
    motorRight.backward();      
    motorLeft.backward();
    Serial.println("Run backward");
    //det = readCommand();
    break;
    
    case 'L':// L, move wheels left
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/4);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    det = readCommand();
    break;
    
    case 'R': // R, move wheels right
    motorRight.setSpeed(vel/4);
    motorLeft.setSpeed(vel);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    det = readCommand();
    break;
    
    case 'I': // I, turn right forward
    motorRight.setSpeed(vel/2);
    motorLeft.setSpeed(vel);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    det = readCommand();
    break;
    
    case 'J': // J, turn right back
    motorRight.setSpeed(vel/2);
    motorLeft.setSpeed(vel);  
    motorRight.run(BACKWARD);     
    motorLeft.run(BACKWARD);
    //det = readCommand();
    break;
    
    case 'G': // G, turn left forward
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/2);
    motorRight.run(FORWARD);      
    motorLeft.run(FORWARD);
    //det = readCommand();
    break;
    
    case 'H': // H, turn left back
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel/2);
    motorRight.run(BACKWARD);     
    motorLeft.run(BACKWARD);
    //det = readCommand();
    break;
    
    case 'S': 
   // S, stop
    motorRight.stop();
    motorLeft.stop();
    //det = readCommand();
    break;
  }
  }

  switch (det){
    case 'w':
    
   Serial.println(" Follow the wall mode");
   BtSerial.println(" Follow the wall mode");
   
  //for wall follower robot.
   motorRight.setSpeed(vel); //set the speed of the motors, between 0-255
   motorLeft.setSpeed (vel); 

  long duration, distance; // start the scan
  //digitalWrite(trigPin, LOW);  
  //delayMicroseconds(2); // delays are required for a succesful sensor operation.
  //digitalWrite(trigPin, HIGH);

  //delayMicroseconds(10); //this delay is required as well!
  //digitalWrite(trigPin, LOW);
  //duration = pulseIn(echoPin, HIGH);
  //distance = (duration/2) / 29.1;// convert the distance to centimeters.
  
  distance = sonar.ping_cm();
  Serial.println("distance "+distance);
  
  if (distance < 30)/*The distance that need to to keep with the wall */ {   
Serial.println ("Wall is ditected!" );
Serial.println (" Started following the wall ");
Serial.println (" Turning !");
   motorRight.setSpeed(vel);
   motorLeft.stop();
   motorRight.run(FORWARD);
   delay(500); // wait for a second
}

  else {
   Serial.println ("No Wall detected. turning round");
   delay (15);
    motorRight.stop();
    motorLeft.setSpeed(vel); 
    motorLeft.run (FORWARD);

  }
     break;
     
    case 'a':

   Serial.println(" obstacle avoider robot");
   BtSerial.println(" obstacle avoider robot");
  //obstacle avoider robot
  
   motorRight.setSpeed(vel); //set the speed of the motors, between 0-255
   motorLeft.setSpeed (vel);  
 
  long Aduration, Adistance; // start the scan
  //digitalWrite(trigPin, LOW);  
  //delayMicroseconds(2); // delays are required for a succesful sensor operation.
  //digitalWrite(trigPin, HIGH);

  //delayMicroseconds(10); //this delay is required as well!
  //digitalWrite(trigPin, LOW);
  //Aduration = pulseIn(echoPin, HIGH);
  //Adistance = (Aduration/2) / 29.1;// convert the distance to centimeters.
  
  Adistance = sonar.ping_cm();
  Serial.println("distance "+Adistance);
  
  if (Adistance < 25)/*if there's an obstacle 25 centimers, ahead, do the following: */ {   
  Serial.println ("Close Obstacle detected!" );
  Serial.println ("Obstacle Details:");
  Serial.print ("Distance From Robot is " );
  Serial.print ( Adistance);
  Serial.print ( " CM!");// print out the distance in centimeters.

Serial.println (" The obstacle is declared a threat due to close distance. ");
Serial.println (" Turning !");
    motorRight.setSpeed(vel);
    motorLeft.setSpeed(vel); 
    motorLeft.run(BACKWARD);  // Turn as long as there's an obstacle ahead.
    motorRight.run (FORWARD);

}
  else {
   Serial.println ("No obstacle detected. going forward");
   delay (15);
   motorRight.setSpeed(vel);
   motorLeft.setSpeed(vel);
   motorRight.run(FORWARD); //if there's no obstacle ahead, Go Forward! 
   motorLeft.run(FORWARD);  

    } 
   break;
  }
  
}

//get bluetooth code received from serial port
int readCommand(){
  //if (Serial.available() > 0){// if there is valid data in the serial port
  //dataIn = Serial.read();// stores data into a varialbe

if (BtSerial.available()) {// if there is valid data in the serial port
  dataIn = BtSerial.read();// stores data into a varialb
   Serial.print("Command: ");
   Serial.println(dataIn); 

//decode the command
    if (dataIn == 'F'){//Forward
      determinant = 'F';
    }
    else if (dataIn == 'B'){//Backward
      determinant = 'B';
    }
    else if (dataIn == 'L'){//Left
      determinant = 'L';
    }
    else if (dataIn == 'R'){//Right
      determinant = 'R';
    }
    else if (dataIn == 'I'){//Froward Right
      determinant = 'I';
    }
    else if (dataIn == 'J'){//Backward Right
      determinant = 'J';
    }
    else if (dataIn == 'G'){//Forward Left
      determinant = 'G';
    }    
    else if (dataIn == 'H'){//Backward Left
      determinant = 'H';
    }
    else if (dataIn == 'S'){//Stop
      determinant = 'S';
    }
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
    }   
    else if (dataIn == 'w'){//Extra On
      determinant = 'w';
    }
    else if (dataIn == 'a'){//Extra On
      determinant = 'a';
    }

  }
  return determinant;
}
