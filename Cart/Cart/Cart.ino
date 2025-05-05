#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "GIOVANNI-XPS";
char pass[] = "zD9*9395";

char server[] = "192.168.137.1";  // IP of PC acting as bridge
int port = 5001;

WiFiClient client;

void setup() {
  pinMode(13,OUTPUT);
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
    client.println("Hello from Arduino");
  } else {
    Serial.println("Connection to server failed");
  }
}

void loop() {
  if (client.connected()) {
    if (client.available()) {
        Serial.println("Starting cart");
        client.read();
        digitalWrite(LED_BUILTIN,HIGH);
}
  } else {
    Serial.println("Disconnected. Reconnecting...");
    client.connect(server, port);
  }
}
