/*
Ultrasonic Sensor Code
Robotics Tutorial
*/
#include <Servo.h> 

const int TRIG_PIN  = 13;
const int ECHO_PIN = 12;
const int SERVO_PIN = 9;
const int baud = 9600;

int STOP = 90;
int GO = 165;
int BACK = 15; 


Servo myservo;  // create servo object to control a servo

void setup() {
Serial.begin(baud);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
myservo.write(STOP);
}

void loop() {
long distance_cm = PING();
Serial.print(distance_cm);  // print distance value
Serial.println(" cm");      // print units and start newline

if (distance_cm <= 10){
  myservo.write(BACK);
  delay(15);
  }
else if (distance_cm <= 25){
    myservo.write(STOP);
  delay(15);
  }
else {
  myservo.write(GO);
  delay(15);
  }
delay(250);
}

long PING() {
  long distance;
  long duration;
  digitalWrite(TRIG_PIN, LOW);  // Emitting Low frequency wave
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); // Emitting High frequency wave
  delayMicroseconds(10);
 digitalWrite(TRIG_PIN, LOW);   // Emitting Low frequency wave
 duration = pulseIn(ECHO_PIN, HIGH);  // Listening for Pulse waves to return
 distance = duration/58; // calculating distance (in cm) based on speed of sound through air (29 us/cm)
 return distance;
}


