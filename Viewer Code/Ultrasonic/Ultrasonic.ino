// Code for interfacing arduino ultrasonic sensor with python to store data in csv file on computer

// Define pins
const int trigPin1 = 13;
const int echoPin1 = 12;
const int trigPin2 = 11;
const int echoPin2 = 10;
// Define Baud speed
const int BAUD =  9600;
// Define length of buffers 
const int data_length = 128;
const int output_length = 256;
// Create buffers
char DATA_BUFFER[data_length];
char OUTPUT_BUFFER[output_length];
// Define offsets
const int offset1in = 4;
const int offset1cm = 10.16;
const int offset2in = 4;
const int offset2cm = 10.16;


void setup() {
Serial.begin (BAUD);      // set Baud rate
pinMode(trigPin1, OUTPUT); // set trig pin mode to OUTPUT
pinMode(echoPin1, INPUT);  // sest echo pin mode to INPUT
pinMode(trigPin2, OUTPUT); // set trig pin mode to OUTPUT
pinMode(echoPin2, INPUT);  // sest echo pin mode to INPUT
}

void loop() {
long duration1 = PING(trigPin1,echoPin1); // get data buffer from ultrasonic sensor
long duration2 = PING(trigPin2,echoPin2); // get data buffer from ultrasonic sensor 2
// Calculate Distance from time 
long cm_distance1;   // create distance variable for centimeters for sensor 1
long in_distance1;   // create distance variable for inches for sensor 1
long cm_distance2;   //create distance variable for centimeters for sensor 2
long in_distance2;   // create distance variable for inches for sensor 2
long filtered_distance;   // create distance variable for inches for sensor 2
cm_distance1 = duration1/58+offset1cm;  //calculate distance(in centimeters) based on speed of sound
in_distance1 = duration1/148+offset1in; //calculate distance(in inches) based on speed of sound
cm_distance2 = duration2/58+offset2cm;  //calculate distance(in centimeters) based on speed of sound
in_distance2 = duration2/148+offset2in; //calculate distance(in inches) based on speed of sound

//Switching between sensor 1 and sensor 2

if (in_distance1 <= 48)
{
  filtered_distance = in_distance1;
}
else
{
  filtered_distance = 96 - in_distance2;
}

// Format and print Data 
sprintf(DATA_BUFFER,"{'duration1':%ld,'distance1(cm)':%ld,'distance1(in)':%ld,'duration2':%ld,'distance2(cm)':%ld,'distance2(in)':%ld,'filtered_distance':%ld}", duration1, cm_distance1, in_distance1, duration2, cm_distance2, in_distance2, filtered_distance);
int chksum = checksum(DATA_BUFFER);
sprintf(OUTPUT_BUFFER, "{'data':%s,'chksum':%d}", DATA_BUFFER, chksum);
Serial.println(OUTPUT_BUFFER);
Serial.flush();

delay(1000);
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

int checksum(char* buf){
  int sum = 0;
  for(int i = 0; i < data_length; i++){
    sum += buf[i];    
  }
  return sum % 256;
}





