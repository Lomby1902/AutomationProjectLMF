#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>

char ssid[] = "GIOVANNI-XPS";
char pass[] = "zD9*9395";

#define TRG_PIN_1 7
#define TRG_PIN_2 8
#define ECHO_PIN_1 2
#define ECHO_PIN_2 3

volatile uint8_t trig_flag_1 = 0;
volatile uint8_t echo_flag_1 = 0;
volatile uint8_t trig_flag_2 = 0;
volatile uint8_t echo_flag_2 = 0;
volatile unsigned long echoStart_1 = 0;
volatile unsigned long echoEnd_1 = 0;
volatile unsigned long echoStart_2 = 0;
volatile unsigned long echoEnd_2 = 0;
float distance_1 = 0;
float distance_2 = 0;
unsigned long duration_1 = 0;
unsigned long duration_2 = 0;

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position


void echo1(){
  switch(digitalRead(ECHO_PIN_1)){
    case HIGH:
      echoStart_1 = micros();
      break;
    case LOW:
      echoEnd_1 = micros();
      //set falling edge flag
       echo_flag_1 = 1;
  }
}


void echo2(){
  switch(digitalRead(ECHO_PIN_2)){
    case HIGH:
      echoStart_2 = micros();
      break;
    case LOW:
      echoEnd_2 = micros();
      //set falling edge flag
       echo_flag_2 = 1;
  }
}
  



char server[] = "192.168.137.1";  // IP of PC acting as bridge
int port = 5001;

WiFiClient client;

void setup() {
  pinMode(TRG_PIN_1, OUTPUT);
  pinMode(TRG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(ECHO_PIN_2, INPUT);
  
  Serial.begin(9600);

  

  
  while (!Serial);

  int status = WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");

  if (client.connect(server, port)) {
    Serial.println("Connected to server!");
  } else {
    Serial.println("Connection to server failed");
  }

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
    attachInterrupt(digitalPinToInterrupt(ECHO_PIN_1), echo1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_2), echo2, CHANGE);
}

void loop() {
  if (client.connected()) {
    
    //Sensor1: If a pulse was not send, send it
    if(!trig_flag_1){
        digitalWrite(TRG_PIN_1, LOW);
        delayMicroseconds(2);
    
        digitalWrite(TRG_PIN_1, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRG_PIN_1, LOW);
        trig_flag_1 = 1;
    }   
    //Sensor1: If echo has been received
    if (echo_flag_1){
        duration_1 = echoEnd_1 - echoStart_1;
        //s = v*t, sound speed is 343 m/s, wave reaches object and comes back
        distance_1 = .0343*(duration_1/2);
        if(distance_1 < 5){
            Serial.println("Arrived to target");
            client.write("Target");
           
        }
         echo_flag_1 = 0;
         trig_flag_1 = 0;
    }  


    //Sensor2: If a pulse was not send, send it
    if(!trig_flag_2){
        digitalWrite(TRG_PIN_2, LOW);
        delayMicroseconds(2);
    
        digitalWrite(TRG_PIN_2, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRG_PIN_2, LOW);
        trig_flag_2 = 1;
    }   
    //Sensor2: If echo has been received
    if (echo_flag_2){
        duration_2 = echoEnd_2 - echoStart_2;
        //s = v*t, sound speed is 343 m/s, wave reaches object and comes back
        distance_2 = .0343*(duration_2/2);
        if(distance_2 < 5){
            Serial.println("Object placed");
           
        }
         echo_flag_2 = 0;
         trig_flag_2 = 0;
    }  


   
   if (client.available()) {
        Serial.println("Starting cart");
        client.read();
        digitalWrite(LED_BUILTIN,HIGH);
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
    }
  } else {
    Serial.println("Disconnected. Reconnecting...");
    client.connect(server, port);
  }
  delay(10);
}
