#include <Servo.h> 
Servo myservo;

// Define Functions and Variables 

int pos;
const int TRIG_PIN  = 13;
const int ECHO_PIN = 12;
const int SERVO_PIN = 9;
const int baud = 9600;

long PING(); 

void setup() {
pos = 0;
Serial.begin(baud);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
myservo.attach(SERVO_PIN);
myservo.write(pos);
}

void loop() {
long distance_cm = PING(); 
Serial.print(distance_cm);
Serial.println(" cm");
//pos = 0;
//myservo.write(pos);

  if (distance_cm < 5 && pos < 5)
    {
      for (pos = 0; pos <= 30; pos += 1) 
        {
          myservo.write(pos);
          delay(50); 
        }

        
    }
    }

/***
    while(pos >=25 && distance_cm < 5)
      {
        long distance_cm = PING(); 
        pos = 30;
        myservo.write(pos);
      }

     if(pos >=25 && distance_cm > 5)
     {
       for (pos = 30; pos >= 0; pos -= 1) 
        {
          myservo.write(pos);
          delay(60); 
        }
***/
  

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
