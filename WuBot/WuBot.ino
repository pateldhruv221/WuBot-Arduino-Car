//libraries 
#include <IRremote.h>
#include <SR04.h>
#include <Servo.h>

//macros
#define ENABLE1 2
#define ENABLE2 4
#define ENABLE3 6
#define ENABLE4 8
#define IN1 35 
#define IN2 33
#define IN3 31
#define IN4 29
#define IN5 43
#define IN6 45
#define IN7 47
#define IN8 49
#define SERVO_PIN 10

#define IR_PIN 44

#define TRIG_PIN 12
#define ECHO_PIN 13 

#define COLLISION_LIMIT 15

SR04 USSensor = SR04(ECHO_PIN,TRIG_PIN);
long USVal = 0, USValLeft = 0, USValRight = 0;

//initial variables
IRrecv IR(IR_PIN);
decode_results cmd;
Servo servo;

String myCom; //letting user know which button is pressed
int motorSpeed = 205, OGMotorSpeed = 0;
int motorSpeedChange = 20;
//stores values for the IN pins
int inVals1[] = {0, 0, 0, 0}; //blank, IN1, IN2, IN3, IN4
int inVals2[] = {0, 0, 0, 0}; //IN values for IN5, IN6, IN7, IN8
long previousVal = 0xFF02FD; //play and pause button

void setup(){
   Serial.begin(9600);
   
   IR.enableIRIn();

   pinMode(ENABLE1,OUTPUT);
   pinMode(ENABLE2,OUTPUT);
   pinMode(ENABLE3,OUTPUT);
   pinMode(ENABLE4,OUTPUT);
   pinMode(IN1,OUTPUT);
   pinMode(IN2,OUTPUT);
   pinMode(IN3,OUTPUT);
   pinMode(IN4,OUTPUT);
   pinMode(IN5,OUTPUT);
   pinMode(IN6,OUTPUT);
   pinMode(IN7,OUTPUT);
   pinMode(IN8,OUTPUT);

   servo.attach(SERVO_PIN);
   servo.write(90);
}

void loop(){
  IR.resume(); //refreshes the IR module
  delay(100);
  checkUSSensor();

  // if something is pressed
  if (IR.decode(&cmd) != 0x000000) {
    //check if it's a "special" value like speed increase or decrease
    if (cmd.value == 0xFF629D){ //volume +, speed increase
      motorSpeed += motorSpeedChange;
      if (motorSpeed >= 255){
        Serial.println("Robot has reached its maximum speed (255)");
        motorSpeed = 255;
        motor();
      }
      else {
        Serial.println("Increase Speed");
        Serial.println(motorSpeed);
        motor();
      }
    } 
    else if (cmd.value == 0xFFA857) { //volume -, speed decrease
      motorSpeed -= motorSpeedChange;
      if(motorSpeed <= 125){
        Serial.println("Robot has reached its minimum speed (125)");
        motorSpeed = 125;
        motor();
      }   
      else {
        Serial.println("Decrease Speed");
        Serial.println(motorSpeed);
        motor();
      }
    } 
    //if a "special" function hasn't been pressed, update the previousVal and the robot's function 
    else {
      previousVal = cmd.value;
      switch(previousVal) {
        case 0xFF906F: //forward
          Serial.println(cmd.value,HEX);
          myCom = "Move Foward (Up)";
          Serial.println(myCom);
          inVals1[0] = 1;
          inVals1[1] = 0;
          inVals1[2] = 0;
          inVals1[3] = 1;
      
          inVals2[0] = 0;
          inVals2[1] = 1;
          inVals2[2] = 1;
          inVals2[3] = 0;
          motor();
          break;
        case 0xFFE01F: //backwards
          Serial.println(cmd.value,HEX);
          myCom = "Move Backward (Down)";
          Serial.println(myCom);
          inVals1[0] = 0;
          inVals1[1] = 1;
          inVals1[2] = 1;
          inVals1[3] = 0;
      
          inVals2[0] = 1;
          inVals2[1] = 0;
          inVals2[2] = 0;
          inVals2[3] = 1;
          motor();
          break;
        case 0xFFC23D: //right
          Serial.println(cmd.value,HEX);
          myCom = "Move Right (Forward button)";
          Serial.println(myCom);
          inVals1[0] = 0;
          inVals1[1] = 0;
          inVals1[2] = 0;
          inVals1[3] = 1;
      
          inVals2[0] = 0;
          inVals2[1] = 1;
          inVals2[2] = 0;
          inVals2[3] = 0;
          motor();
          break;
        case 0xFF22DD: //left
          Serial.println(cmd.value,HEX);
          myCom = "Move Left (Rewind button)";
          Serial.println(myCom);
          inVals1[0] = 1;
          inVals1[1] = 0;
          inVals1[2] = 0;
          inVals1[3] = 0;
      
          inVals2[0] = 0;
          inVals2[1] = 0;
          inVals2[2] = 1;
          inVals2[3] = 0;
          motor();
          break;
        case 0xFF02FD: //play/pause button pressed
          Serial.println(cmd.value,HEX);
          myCom = "STOP";
          Serial.println(myCom);
          inVals1[0] = 0;
          inVals1[1] = 0;
          inVals1[2] = 0;
          inVals1[3] = 0;
      
          inVals2[0] = 0;
          inVals2[1] = 0;
          inVals2[2] = 0;
          inVals2[3] = 0;
          motor();
          break;
    
        case 0xFF6897: //decrease speed gradually (0)
          Serial.println(cmd.value,HEX);
          myCom = "Decrease speed gradually";
          Serial.println(myCom);
    
          for (int z = motorSpeed; z >= 125; z -= 10){
            motorSpeed = z;
            Serial.println(motorSpeed);
            motor();
            delay(1000);
          }
          break;
    
        case 0xFF30CF: //rotate counter-clockwise (1)
          Serial.println(cmd.value, HEX);
          myCom = "Rotate counter-clockwise";
          Serial.println(myCom);
          inVals1[0] = 1;
          inVals1[1] = 0;
          inVals1[2] = 1;
          inVals1[3] = 0;
      
          inVals2[0] = 1;
          inVals2[1] = 0;
          inVals2[2] = 1;
          inVals2[3] = 0;
          motor();
          break;
    
        case 0xFF18E7: //rotate clockwise (2)
          Serial.println(cmd.value, HEX);
          myCom = "Rotate clockwise";
          Serial.println(myCom);
          inVals1[0] = 0;
          inVals1[1] = 1;
          inVals1[2] = 0;
          inVals1[3] = 1;
      
          inVals2[0] = 0;
          inVals2[1] = 1;
          inVals2[2] = 0;
          inVals2[3] = 1;
          motor();
          break;
    
        case 0xFF52AD: //increase speed gradually (9)
          Serial.println(cmd.value,HEX);
          myCom = "Increase speed gradually";
          Serial.println(myCom);
    
          for (int x = motorSpeed; x <= 255; x += 10){
            motorSpeed = x;
            Serial.println(motorSpeed);
            motor();
            delay(1000);
          }
          break;
        default:
          Serial.println("The command was not recognized");
          break;
      }
    }
  }
  
  
  delay(100);
}

void motor(){
  analogWrite(ENABLE1, motorSpeed);
  digitalWrite(IN1,inVals1[0]);
  digitalWrite(IN2,inVals1[1]);
  analogWrite(ENABLE2, motorSpeed);
  digitalWrite(IN3,inVals1[2]);
  digitalWrite(IN4,inVals1[3]);

  analogWrite(ENABLE3, motorSpeed);
  digitalWrite(IN5,inVals2[0]);
  digitalWrite(IN6,inVals2[1]);
  analogWrite(ENABLE4, motorSpeed);
  digitalWrite(IN7,inVals2[2]);
  digitalWrite(IN8,inVals2[3]);
}

void checkUSSensor(){
  USVal = USSensor.Distance();
  Serial.println(USVal);

  if (USVal < COLLISION_LIMIT) { //only do something if robot is too close to something
    // stop the robot
    inVals1[0] = 0;
    inVals1[1] = 0;
    inVals1[2] = 0;
    inVals1[3] = 0; 
    inVals2[0] = 0;
    inVals2[1] = 0;
    inVals2[2] = 0;
    inVals2[3] = 0;
    motor();
    Serial.println("The robot was about to hit something! Motion stopped!");
    delay(2500);
    //checks left
    servo.write(0);
    USValRight = USSensor.Distance();
    Serial.println(USValRight);
    delay(1500);
    //checks right
    servo.write(180);
    USValLeft = USSensor.Distance();
    Serial.println(USValLeft);
    delay(1500);
    
    //servo goes back to original position
    servo.write(90);
    delay(200);

    //make it go back
    Serial.println("move back");
    inVals1[0] = 0;
    inVals1[1] = 1;
    inVals1[2] = 1;
    inVals1[3] = 0;

    inVals2[0] = 1;
    inVals2[1] = 0;
    inVals2[2] = 0;
    inVals2[3] = 1;
    motor();
    delay(1000);

    

    //*in the rare case both vals are equal, the robot will just turn left
    if (USValLeft >= USValRight){
      Serial.println("Rotate left");
      inVals1[0] = 1;
      inVals1[1] = 0;
      inVals1[2] = 1;
      inVals1[3] = 0;
  
      inVals2[0] = 1;
      inVals2[1] = 0;
      inVals2[2] = 1;
      inVals2[3] = 0;     
    } 
    
    else if (USValRight > USValLeft) {
      Serial.println("Rotate right");
      inVals1[0] = 0;
      inVals1[1] = 1;
      inVals1[2] = 0;
      inVals1[3] = 1;
  
      inVals2[0] = 0;
      inVals2[1] = 1;
      inVals2[2] = 0;
      inVals2[3] = 1;
    }
    OGMotorSpeed = motorSpeed;
    motorSpeed = 205;
    motor();
    delay(1500); //placeholder value, time it takes for rotate
    //stops after pointing the right direction
    //the user is free to control the robot from hereon out
    Serial.println("Robot control access has been handed back to the user! OH YEAH!");
    inVals1[0] = 0;
    inVals1[1] = 0;
    inVals1[2] = 0;
    inVals1[3] = 0; 
    inVals2[0] = 0;
    inVals2[1] = 0;
    inVals2[2] = 0;
    inVals2[3] = 0;
    motorSpeed = OGMotorSpeed;
    motor();
    
    
  }
}
