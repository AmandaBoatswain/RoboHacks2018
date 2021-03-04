#include<Servo.h>

/*Put here the pin where
the Servomotor is connected
Remember it should be
marked with ~ sign */
int pin=6;
Servo my_servo;

String message;
char initial;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  my_servo.attach(pin);
  my_servo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    message=Serial.readString();
    initial=message.charAt(0);
    Serial.println(message);
  }
  switch(initial){
    case 'a':
      my_servo.write(0);
      break;
    case 'b':
      my_servo.write(90);
      break;
    case 'c':
      my_servo.write(180-my_servo.read());
      break;
    case 'd':
      while(Serial.available()<=0){
        my_servo.write(180-my_servo.read());
        delay(5000);
      }
      break;
    case 'e':
      while(Serial.available()<=0){
        for(int i=90;i<180;i++){
          my_servo.write(i);
          delay(50);  
        }
        for(int i=90;i>0;i--){
          my_servo.write(i);
          delay(50);  
        }
      }
      break;
    case 'f':
      while(Serial.available()<=0){
        for(int i=0;i<180;i++){
          my_servo.write(i);
          delay(50);  
        }
        for(int i=180;i>0;i--){
          my_servo.write(i);
          delay(50);  
        }
      }
      break;
    default:
      break;
  }
  initial=' ';
  delay(10);
}
