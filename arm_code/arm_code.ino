/*************************************************** 
 Function for writing servo arm positions for RoboHacks2018 
 These drivers use I2C to communicate, 2 pins are required to interface.
 ****************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// COMMAND LIST 
const int OPEN_ARM_COMMAND     = 'O'; // Open the arm gripper
const int RAISE_ARM_COMMAND    = 'R'; // Raises the arm 
const int FOLLOW_LINE_COMMAND  = 'D';
const int POUR_DRINK_COMMAND   = 'P'; // Turns the servo motor to pour a drink 

// Servo Motors 
const int OPEN_ARM_SERVO = 0; 
const int RAISE_ARM_SERVO = 1; 
const int POUR_DRINK_SERVO = 2;

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

#define OPEN_SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define OPEN_SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

#define RAISE_SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define RAISE_SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

#define POUR_SERVOMIN  120 // this is the 'minimum' pulse length count (out of 4096)
#define POUR_SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

void setup() {
  Serial.begin(9600);
  Serial.println("Setting starting positions - Servo test!");

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  pwm.setPWM(OPEN_ARM_SERVO, 0, OPEN_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  pwm.setPWM(RAISE_ARM_SERVO, 0, RAISE_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  pwm.setPWM(POUR_DRINK_SERVO, 0, POUR_SERVOMIN); // These will try to 'zero' the servos, ie. not moving 
  
  delay(10);
}

// Robot Functions 

void setOpenServo(int start_pos) {
  pwm.setPWM(OPEN_ARM_SERVO, 0, start_pos);
  //delay(500);
  //pwm.setPWM(OPEN_ARM_SERVO, 0, end_pos);
}

void setRaiseServo(int start_pos) {
  pwm.setPWM(RAISE_ARM_SERVO, 0, start_pos);
  //delay(500);
  //pwm.setPWM(RAISE_ARM_SERVO, 0, end_pos);
}

void setPourServo(int start_pos) {
  pwm.setPWM(POUR_DRINK_SERVO, 0, start_pos);
  //delay(500);
  //pwm.setPWM(POUR_DRINK_SERVO, 0, end_pos);
}

void loop() {
  // Drive each servo one at a time
if (Serial.available() > 0) {
    char command = Serial.read();
    int value = Serial.parseInt();

    switch (command) {
      case OPEN_ARM_COMMAND:
        setOpenServo(300);
        
        delay(1000);
        setOpenServo(OPEN_SERVOMIN);
        break;

       case RAISE_ARM_COMMAND:
        setRaiseServo(400);
        delay(1000);
        setRaiseServo(RAISE_SERVOMIN);
        break;
        
       case POUR_DRINK_COMMAND:
        setPourServo(400);
        delay(1000);
        setPourServo(POUR_SERVOMIN);
        break;
        }

    Serial.println(command);
    Serial.flush();
    }
}

