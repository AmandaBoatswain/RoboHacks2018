/*************************************************** 
 Function for writing servo arm positions for RoboHacks2018 
 These drivers use I2C to communicate, 2 pins are required to interface.
 ****************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// COMMAND LIST 
const int CLOSE_GRIPPER_COMMAND  = 'C'; // Open the arm gripper
const int RAISE_ARM_COMMAND      = 'A'; // Raises the arm 
const int RAISE_ARM_COMMAND2      = 'B'; // Raises the arm 
const int POUR_DRINK_COMMAND     = 'P'; // Turns the servo motor to pour a drink 
const int GRIPPER_ARM_COMMAND    = 'G'; 
const int SET_RAISE_ARM_COMMAND  = 'S'; //   
const int SET_RAISE_ARM_COMMAND2  = 'T'; //  

// Define Variables 
// Servo Motors 
const int GRIPPER_SERVO = 0; 
const int RAISE_ARM_SERVO = 1; 
const int RAISE_ARM_SERVO2 = 3; 
const int POUR_DRINK_SERVO = 2;
int pos; // pos is open Arm servo 

// Ultrasonic sensor 
const int TRIG_PIN  = 7;
const int ECHO_PIN = 6;
const int baud = 9600;

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

// Variables 
// Servo mins & maxs 
#define GRIPPER_SERVOMIN  180  // this is the 'minimum' pulse length count (out of 4096)
#define GRIPPER_SERVOMAX  500 // this is the 'maximum' pulse length count (out of 4096)

#define RAISE_SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define RAISE_SERVOMAX  320 // this is the 'maximum' pulse length count (out of 4096)

#define RAISE_SERVOMIN2  0 // this is the 'minimum' pulse length count (out of 4096)
#define RAISE_SERVOMAX2  60 // this is the 'maximum' pulse length count (out of 4096)

#define POUR_SERVOMIN  220 // this is the 'minimum' pulse length count (out of 4096)
#define POUR_SERVOMAX  450 // this is the 'maximum' pulse length count (out of 4096)

/* --- Prototypes --- */

void set_wheel_servos(int, int, int, int);
int setGripperServo(void); 
int setRaiseServo(void); 
int setRaiseServo2(void); 
int setPourServo(void);
int closeGripper(void);  
long PING();
char command;


void setup() {
  Serial.begin(baud);
  Serial.println("Setting starting positions - Servo test!");

  // Ultrasonic 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
 
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pwm.setPWM(GRIPPER_SERVO, 0, GRIPPER_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  pwm.setPWM(RAISE_ARM_SERVO, 0, RAISE_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  pwm.setPWM(RAISE_ARM_SERVO2, 0, RAISE_SERVOMIN2); // These will try to 'zero' the servos, ie. not moving 
  pwm.setPWM(POUR_DRINK_SERVO, 0, POUR_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  delay(60);
}

void loop() {
   long distance_cm = PING(); 
   //Serial.print(distance_cm);
   //Serial.println(" cm");
   
   if (Serial.available() > 0) {
      char command = Serial.read();
      int value = Serial.parseInt();

    switch (command) {
      
      case CLOSE_GRIPPER_COMMAND:
      //if (distance_cm < 5 && pos < 5){ // If an object is close 
      closeGripper();
        break;

       case GRIPPER_ARM_COMMAND:
        setGripperServo(GRIPPER_SERVOMAX);
        delay(1000);
        setGripperServo(GRIPPER_SERVOMIN);
        break;

       case RAISE_ARM_COMMAND:
          for (pos = RAISE_SERVOMIN ; pos <= RAISE_SERVOMAX; pos += 1){ 
          setRaiseServo(pos);
          Serial.print(pos); 
          }
          break; 
          
          

      case SET_RAISE_ARM_COMMAND:
        setRaiseServo(RAISE_SERVOMAX);
        delay(3000);
        setRaiseServo(RAISE_SERVOMIN);
        break;

        case RAISE_ARM_COMMAND2:
          for (pos = RAISE_SERVOMIN2 ; pos <= RAISE_SERVOMAX2; pos += 1){ 
          setRaiseServo2(pos);
          Serial.print(pos); 
          delay(30);
          }

      case SET_RAISE_ARM_COMMAND2:
        setRaiseServo(RAISE_SERVOMAX2);
        delay(3000);
        setRaiseServo(RAISE_SERVOMIN2);
        break;
        
       case POUR_DRINK_COMMAND:
          for (pos = POUR_SERVOMIN ; pos <= POUR_SERVOMAX; pos += 1){ 
          setPourServo(pos);
          Serial.print(pos); 
          delay(30);
          }
        }

    Serial.println(command);
    Serial.flush();
    }
}


// Robot Functions 

void setGripperServo(int start_pos) {
  pwm.setPWM(GRIPPER_SERVO, 0, start_pos);
}

void setRaiseServo(int start_pos) {
  pwm.setPWM(RAISE_ARM_SERVO, 0, start_pos);
}

void setRaiseServo2(int start_pos) {
  pwm.setPWM(RAISE_ARM_SERVO2, 0, start_pos);
}


void setPourServo(int start_pos) {
  pwm.setPWM(POUR_DRINK_SERVO, 0, start_pos);
}

int closeGripper(void){
      for (pos = GRIPPER_SERVOMIN ; pos <= GRIPPER_SERVOMAX; pos += 1){ 
          setGripperServo(pos);
          Serial.print(pos); 
          delay(10);
      }
      }
          
        
long PING(){
  long distance;
  long duration;
  digitalWrite(TRIG_PIN, LOW); // Emitting Low frequency wave
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);// Emitting High frequency wave
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW); // Emitting Low frequency wave
  duration = pulseIn(ECHO_PIN, HIGH); // Listening for Pulse waves to return
  distance = duration/58; // calculating distance (in cm) based on speed of sound through air (29 us/cm)
  return distance;
  }
