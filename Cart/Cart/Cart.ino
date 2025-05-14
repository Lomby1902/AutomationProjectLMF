#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>

// WiFi credentials
char ssid[] = "Zancle E-Drive";
char pass[] = "7EC446D837";

// Server info
char server[] = "192.168.209.122";  // IP of PC/server
int port = 5001;

WiFiClient client;

// Servo motors
Servo motor1;
Servo motor2;

// Sensor pins
#define TRG_PIN_1 7
#define TRG_PIN_2 8
#define ECHO_PIN_1 2
#define ECHO_PIN_2 3

// Flags and timing for sensors
volatile uint8_t trig_flag_1 = 0;
volatile uint8_t echo_flag_1 = 0;
volatile uint8_t trig_flag_2 = 0;
volatile uint8_t echo_flag_2 = 0;
volatile unsigned long echoStart_1 = 0;
volatile unsigned long echoEnd_1 = 0;
volatile unsigned long echoStart_2 = 0;
volatile unsigned long echoEnd_2 = 0;

unsigned long duration_1 = 0;
unsigned long duration_2 = 0;
float distance_1 = 0;
float distance_2 = 0;


unsigned long lastTrigTime_1 = 0;
unsigned long lastTrigTime_2 = 0;
const unsigned long echoTimeout = 100;  // ms

// State machine for cart behavior
enum CartState {
  WAIT_FOR_START,
  MOVING_TO_TARGET,
  WAITING_FOR_OBJECT,
  RETURNING,
  IDLE
};

CartState cartState = WAIT_FOR_START;
unsigned long returnStart = 0; // For timing return movement

// Interrupt Service Routines
void echo1() {
  switch (digitalRead(ECHO_PIN_1)) {
    case HIGH:
      echoStart_1 = micros();
      break;
    case LOW:
      echoEnd_1 = micros();
      echo_flag_1 = 1;
  }
}

void echo2() {
  switch (digitalRead(ECHO_PIN_2)) {
    case HIGH:
      echoStart_2 = micros();
      break;
    case LOW:
      echoEnd_2 = micros();
      echo_flag_2 = 1;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Motor pins
  motor1.attach(5);
  motor2.attach(6);

  // Sensor pin modes
  pinMode(TRG_PIN_1, OUTPUT);
  pinMode(TRG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(ECHO_PIN_2, INPUT);

  // Built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_1), echo1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN_2), echo2, CHANGE);

  // WiFi connection
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

  stopMotors(); // Ensure motors are stopped at startup
}

void loop() {
  // Reconnect if client disconnects
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting...");
    client.connect(server, port);
    delay(500);
    return;
  }


  // Handle server message (start signal)
  if (client.available()) {
    Serial.println("Start signal received");
    client.read();  // Clear message
    digitalWrite(LED_BUILTIN, HIGH);
    cartState = MOVING_TO_TARGET;
  }

  Serial.println(cartState);
  // State machine logic
  switch (cartState) {
    case WAIT_FOR_START:
      stopMotors();
      break;

    case MOVING_TO_TARGET:
      moveForward();
      // Trigger ultrasonic sensors
      if (!trig_flag_1) {
        digitalWrite(TRG_PIN_1, LOW);
        delayMicroseconds(2);
        digitalWrite(TRG_PIN_1, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRG_PIN_1, LOW);
        trig_flag_1 = 1;
        lastTrigTime_1 = millis();
      }

      // Process echo sensor 1 (target)
      if (echo_flag_1) {
        Serial.println("Echo1");
        duration_1 = echoEnd_1 - echoStart_1;
        distance_1 = 0.0343 * (duration_1 / 2);
        echo_flag_1 = 0;
        trig_flag_1 = 0;
      }

      // Timeout for sensor 1
      if (trig_flag_1 && (millis() - lastTrigTime_1 >= echoTimeout)) {
        Serial.println("Sensor1 timeout");
        trig_flag_1 = 0;
        echo_flag_1 = 0;
      }


      if (distance_1 < 5) {
          Serial.println("Arrived to target");
          client.write("Target");
          stopMotors();
          cartState = WAITING_FOR_OBJECT;
        }
      break;

    case WAITING_FOR_OBJECT:
      if (!trig_flag_2) {
        digitalWrite(TRG_PIN_2, LOW);
        delayMicroseconds(2);
        digitalWrite(TRG_PIN_2, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRG_PIN_2, LOW);
        trig_flag_2 = 1;
        lastTrigTime_2 = millis();
      }



      // Process echo sensor 2 (object)
      if (echo_flag_2) {
        Serial.println("Echo2");
        duration_2 = echoEnd_2 - echoStart_2;
        distance_2 = 0.0343 * (duration_2 / 2);
        echo_flag_2 = 0;
        trig_flag_2 = 0;
      }

      // Timeout for sensor 2
      if (trig_flag_2 && (millis() - lastTrigTime_2 >= echoTimeout)) {
        Serial.println("Sensor2 timeout");
        trig_flag_2 = 0;
        echo_flag_2 = 0;
      }
      if (distance_2 < 5) {
        Serial.println("Object placed, returning");
        delay(8000);
        moveBackward();
        returnStart = millis();  // Start return timer
        cartState = RETURNING;
      }
      break;

    case RETURNING:
      if (millis() - returnStart >= 2000) {
        Serial.println("Return complete. Stopping.");
        stopMotors();
        digitalWrite(LED_BUILTIN, LOW);
        cartState = IDLE;
      }
      break;

    case IDLE:
      // Reset to initial state
      cartState = WAIT_FOR_START;
      break;
  }

  delay(10); // Reduce loop frequency

  client.write("Arduin");
}

// -----------------
// Helper Functions
// -----------------

void moveForward() {
  motor1.write(70);   // Calibrate this if needed
  motor2.write(110);
}

void moveBackward() {
  motor1.write(110);  // Calibrate this if needed
  motor2.write(70);
}

void stopMotors() {
  motor1.write(90);   // Use calibrated stop value if necessary
  motor2.write(90);
}
