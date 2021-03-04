// ultrasonic flag

int sonic = 1;    // 0 means we are in first half, 1 means second half
int row_pos = 0;  // keeps track of what row we are in


/* --- Libraries --- */
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <RunningMedian.h>

/* --- Prototypes --- */

void set_wheel_servos(int, int, int, int);
int wait(void);
int follow_line(void);
int align(void);
int jump(void);
int line_detect(void);
int zero(void);

/* --- Constants --- */
// Time
const int WAIT_INTERVAL = 100;
const int STEP_INTERVAL = 1000;
const int JUMP_INTERVAL = 3000;
const int TURN90_INTERVAL = 2500;

// Serial Commands
const int BAUD = 9600;
const int OUTPUT_LENGTH = 256;
const int ALIGN_COMMAND         = 'A';
const int DIST_FROM_ROW         = 'B';
const int FUCK_OFF_COMMAND      = 'C';
const int D                     = 'D';
const int FIRST_ROW_COMMAND     = 'E'; //gets you to the first row
const int ADVANCE_ROW_COMMAND   = 'F'; //advance one row
const int G                     = 'G';
const int H                     = 'H';
const int I                     = 'I';
const int JUMP_COMMAND          = 'J';
const int K                     = 'K';
const int LINE_COMMAND          = 'L';
const int CORNER                = 'M';
const int N                     = 'N';
const int O                     = 'O';
const int P                     = 'P';
const int Q                     = 'Q';
const int R                     = 'R';
const int S                     = 'S';
const int TURN_COMMAND          = 'T';
const int U                     = 'U';
const int V                     = 'V';
const int WAIT_COMMAND          = 'W';
const int X                     = 'X';
const int Y                     = 'Y';
const int ZERO_COMMAND          = 'Z';
const int UNKNOWN_COMMAND       = '?';

/// Line Threshold
const int LINE_THRESHOLD = 200; // i.e. 2.5 volts
const int OFFSET_SAMPLES = 5;

/// I/O Pins
const int LEFT_LINE_PIN = A0;
const int CENTER_LINE_PIN = A1;
const int RIGHT_LINE_PIN = A2;
const int TRIG_FRONT = 13;
const int ECHO_FRONT = 12;
const int TRIG_BACK = 11;
const int ECHO_BACK = 10;
// A4 - A5 reserved

// Channels
const int FRONT_LEFT_WHEEL_SERVO = 0;
const int FRONT_RIGHT_WHEEL_SERVO = 1;
const int BACK_LEFT_WHEEL_SERVO = 2;
const int BACK_RIGHT_WHEEL_SERVO = 3;

// Define offsets for ultrasonic sensors (in)
const float offset1in = 4.2; // Back Sensor
const float offset2in = 5.4;   // Front Sensor

// PWM Settings
const int SERVO_OFF = 335; // this is the servo off pulse length
const int PWM_FREQ = 60; // analog servos run at 60 Hz
const int SERVO_SLOW = 10;
const int SERVO_MEDIUM = 20;
const int SERVO_FAST = 30;

// Wheel Calibration Settings
const int FR = 24;
const int FL = 21;
const int BR = 22;
const int BL = 13;

// For Shield #
/* --- Variables --- */
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // called this way, it uses the default address 0x40
RunningMedian line_offset = RunningMedian(OFFSET_SAMPLES);;
char command;
int result;
char output[OUTPUT_LENGTH];

/* --- Helper Functions --- */
// These functions are not actions, but are often called by actions
void set_wheel_servos(int fl, int fr, int bl, int br) {
  pwm.setPWM(FRONT_LEFT_WHEEL_SERVO, 0, SERVO_OFF + fl + FL);
  pwm.setPWM(FRONT_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + fr + FR);
  pwm.setPWM(BACK_LEFT_WHEEL_SERVO, 0, SERVO_OFF + bl + BL);
  pwm.setPWM(BACK_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + br + BR);
}
//int check_switch(void) {
//  return digitalRead(BACKUP_SWITCH_PIN);}
int line_detect(void) {
  int l = analogRead(LEFT_LINE_PIN);
  int c = analogRead(CENTER_LINE_PIN);
  int r = analogRead(RIGHT_LINE_PIN);
  int x;

  if ((l > LINE_THRESHOLD) && (c < LINE_THRESHOLD) && (r < LINE_THRESHOLD)) {
    x = 2; // very off
  }
  else if ((l > LINE_THRESHOLD) && (c > LINE_THRESHOLD) && (r < LINE_THRESHOLD)) {
    x = 1; // midly off
  }
  else if ((l < LINE_THRESHOLD) && (c > LINE_THRESHOLD) && (r < LINE_THRESHOLD)) {
    x = 0; // on target
  }
  else if ((l < LINE_THRESHOLD) && (c > LINE_THRESHOLD) && (r > LINE_THRESHOLD)) {
    x = -1;  // mildy off
  }
  else if ((l < LINE_THRESHOLD) && (c < LINE_THRESHOLD) && (r > LINE_THRESHOLD)) {
    x = -2; // very off
  }
  else if ((l < LINE_THRESHOLD) && (c < LINE_THRESHOLD) && (r < LINE_THRESHOLD)) {
    x = -255; // off entirely
  }
  else {
    x = 255;
  }
  return x;
}

// Tells you how far from a certain row you are (in inches)
float row_finder(float row){

  RunningMedian duration1 = RunningMedian(15);
  RunningMedian duration2 = RunningMedian(15);
  
  for (int i=0;i<15;i++){
    duration1.add(PING(TRIG_BACK,ECHO_BACK)); // get data buffer from Back ultrasonic sensor
    delay(10);
    duration2.add(PING(TRIG_FRONT,ECHO_FRONT)); // get data buffer from Front ultrasonic sensor
    delay(10);
  }
  
  float current_row;
  float row_dist = row*3.0+16.5;
  
  // Calculate Distance from time 
  float in_distance1;   // create distance variable for inches for sensor 1
  float in_distance2;   // create distance variable for inches for sensor 2
  float filtered_distance;   // create distance variable for inches for sensor 2
  in_distance1 = duration1.getAverage(8)/148.0+offset1in; //calculate distance(in inches) based on speed of sound
  in_distance2 = duration2.getAverage(8)/148.0+offset2in; //calculate distance(in inches) based on speed of sound
  
  if (in_distance1 <= 48.0 && sonic ==0){
    Serial.print("Back Sensor: ");
    Serial.println(in_distance1);
    return (row_dist-in_distance1);
    }
  else if(in_distance2<= 48.0){
    sonic =1;
    Serial.print("Front Sensor: ");
    Serial.println(in_distance2);
    return -(96-row_dist-in_distance2);
    }
    else{Serial.print("something went wrong");}

}
long PING(int TP,int EP) {
  long duration;      // create duration variable

// Send Ping
  digitalWrite(TP, LOW); 
  delayMicroseconds(2);
  digitalWrite(TP, HIGH);
  delayMicroseconds(10);
  digitalWrite(TP, LOW);
// Receive Ping
  duration = pulseIn(EP, HIGH);
  return duration;
}


/* --- Setup --- */
void setup() {

  // USB
  Serial.begin(BAUD);

  // Pins
  pinMode(CENTER_LINE_PIN, INPUT);
  pinMode(RIGHT_LINE_PIN, INPUT);
  pinMode(LEFT_LINE_PIN, INPUT);
  pinMode(TRIG_FRONT, OUTPUT); // set trig pin mode to OUTPUT
  pinMode(ECHO_FRONT, INPUT);  // sest echo pin mode to INPUT
  pinMode(TRIG_BACK, OUTPUT); // set trig pin mode to OUTPUT
  pinMode(ECHO_BACK, INPUT);  // sest echo pin mode to INPUT
  
  // PWM
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);  // This is the ideal PWM frequency for servos
  pwm.setPWM(FRONT_LEFT_WHEEL_SERVO, 0, SERVO_OFF + FL);
  pwm.setPWM(FRONT_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + FR);
  pwm.setPWM(BACK_LEFT_WHEEL_SERVO, 0, SERVO_OFF + BL);
  pwm.setPWM(BACK_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + BR);
}

/* --- Loop --- */
// For each command specified in the *--- Prototypes ---* section, it must be specified here
// as a recognized character which is associated with an action function
void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    int value = Serial.parseInt();
    switch (command) {
      case ALIGN_COMMAND:
        result = align();
        break;
      case ADVANCE_ROW_COMMAND:
        result = advance_row();
        break;
      case FIRST_ROW_COMMAND:
        first_row();
        break;
      case FUCK_OFF_COMMAND:
        follow_line();
        result = pivot_right(1000); // this argument must be optomized to make a nice 90 degree turn
        follow_line();
        result = pivot_right(1000); // this argument must be optomized to make a nice 90 degree turn
        follow_line();
        break;
      case WAIT_COMMAND:
        result = wait();
        break;
      case JUMP_COMMAND:
        result = jump();
        break;
      case LINE_COMMAND:
        result = line_detect();
        break;
      case ZERO_COMMAND:
        result = zero();
        break;
      case DIST_FROM_ROW:
        Serial.println(row_finder(13));
        break;
      case CORNER:
        result = pivot_right(5000); // this argument must be optomized to make a nice 90 degree turn
        break;
      default:
        result = 255;
        command = UNKNOWN_COMMAND;
        break;
    }
    sprintf(output, "{'command':'%c','result':%d,'left':%d,'center':%d,'right':%d}", command, result, analogRead(LEFT_LINE_PIN), analogRead(CENTER_LINE_PIN), analogRead(RIGHT_LINE_PIN));
    Serial.println(output);
    Serial.flush();
  }
}

/* --- Action Functions --- */
// These functions are complete actions which are executed by sequentially by robot
// 1. Jump out of the starting zone to the 2nd line
int jump(void) {
  set_wheel_servos(SERVO_FAST, -SERVO_FAST, SERVO_FAST, -SERVO_FAST);
  delay(JUMP_INTERVAL);
  while (line_detect() == -255) {
    delay(WAIT_INTERVAL); // Drive until a line is reached
  }
  set_wheel_servos(0, 0, 0, 0);
  return 0;
}

// 2. Align onto line by wiggling
int align(void) {
  // Rotate to the right
  set_wheel_servos(SERVO_SLOW, SERVO_SLOW, SERVO_SLOW, SERVO_SLOW);
  while (line_detect() != -255) {
    delay(WAIT_INTERVAL);
  }
  // Wiggle
  int i = 0;
  int x = line_detect();
  
  while (i <= 20) {
    if (x == 0) {
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
      i++;
    }
    else if (x == -1) {
      set_wheel_servos(SERVO_MEDIUM, -SERVO_SLOW, SERVO_MEDIUM, -SERVO_SLOW);
      i++;
    }
    else if (x == -2) {
      set_wheel_servos(SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM);
      i = 0;
    }
    else if (x == 1) {
      set_wheel_servos(SERVO_SLOW, -SERVO_MEDIUM, SERVO_SLOW, -SERVO_MEDIUM);
      i++;
    }
    else if (x == 2) {
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
      i = 0;
    }
    else if (x == -255) {
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
      i = 0;
    }
    else if (x == 255) {
      // Turn 90
      //set_wheel_servos(SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM);
      //delay(TURN90_INTERVAL);
      break;
    }
    delay(WAIT_INTERVAL);
    x = line_detect();
  }
  set_wheel_servos(0, 0, 0, 0); // Halt
  return 0;
}

// 3. Follow line to intersection
// #!TODO
int first_row(void) {
  // Search until all line sensors are over tape (i.e. at the end)
  int x = line_detect();
  if (x == 255) {
    while (x == 255) {
      x = line_detect();
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
  }
  while (abs(row_finder(1)>0.25)){
  
    x = line_detect();
    if (x == -1) {
      set_wheel_servos(30, -20, 30, -20);
    }
    else if (x == -2) {
      set_wheel_servos(20, 20, 20, 20);
    }
    else if (x == 1) {
      set_wheel_servos(20, -30, 20, -30);
    }
    else if (x == 2) {
      set_wheel_servos(-20, -20, -20, -20);
    }
    else if (x == 0) {
      set_wheel_servos(30, -30, 30, -30);
    }
    else if (x == 255) {
      break;
    }
    else {
      
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
    delay(50);
}


  set_wheel_servos(0, 0, 0, 0); // Stop servos
  return 0;
  row_pos++; 
}

int advance_row(void) {
  // Search until all line sensors are over tape (i.e. at the end)
  int x = line_detect();
  if (x == 255) {
    while (x == 255) {
      x = line_detect();
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
  }
  while (abs(row_finder(row_pos+1)>0.25)){
  
    x = line_detect();
    if (x == -1) {
      set_wheel_servos(30, -20, 30, -20);
    }
    else if (x == -2) {
      set_wheel_servos(20, 20, 20, 20);
    }
    else if (x == 1) {
      set_wheel_servos(20, -30, 20, -30);
    }
    else if (x == 2) {
      set_wheel_servos(-20, -20, -20, -20);
    }
    else if (x == 0) {
      set_wheel_servos(30, -30, 30, -30);
    }
    else if (x == 255) {
      break;
    }
    else {
      
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
    delay(50);
}


  set_wheel_servos(0, 0, 0, 0); // Stop servos
  row_pos++;
  return 0;
}

int follow_line(void) {
  // Search until all line sensors are over tape (i.e. at the end)
  int x = line_detect();
  if (x == 255) {
    while (x == 255) {
      x = line_detect();
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
  }

  while(true){
 
  
    x = line_detect();
    if (x == -1) {
      set_wheel_servos(30, -20, 30, -20);
    }
    else if (x == -2) {
      set_wheel_servos(20, 20, 20, 20);
    }
    else if (x == 1) {
      set_wheel_servos(20, -30, 20, -30);
    }
    else if (x == 2) {
      set_wheel_servos(-20, -20, -20, -20);
    }
    else if (x == 0) {
      set_wheel_servos(30, -30, 30, -30);
    }
    else if (x == 255) {
      break;
    }
    else {
      
      set_wheel_servos(SERVO_SLOW, -SERVO_SLOW, SERVO_SLOW, -SERVO_SLOW);
    }
    delay(50);
  }
  set_wheel_servos(0, 0, 0, 0); // Stop servos

  
  return 0;
}

// 5. Wait for another action (e.g. transfer) to occur
int wait(void) {
  delay(WAIT_INTERVAL);
  return 0;
}

int pivot_right(int value) {
  set_wheel_servos(SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM, SERVO_MEDIUM);
  delay(value);
  set_wheel_servos(0, 0, 0, 0);
  return 0;
}

int zero(void) {
  pwm.setPWMFreq(PWM_FREQ);  // This is the ideal PWM frequency for servos
  pwm.setPWM(FRONT_LEFT_WHEEL_SERVO, 0, SERVO_OFF + FL);
  pwm.setPWM(FRONT_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + FR);
  pwm.setPWM(BACK_LEFT_WHEEL_SERVO, 0, SERVO_OFF + BL);
  pwm.setPWM(BACK_RIGHT_WHEEL_SERVO, 0, SERVO_OFF + BR);
}

