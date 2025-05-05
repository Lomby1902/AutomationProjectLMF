#include <WiFi.h>

//Wi-Fi credntials
const char* ssid     = "GIOVANNI-XPS 4255";
const char* password = "zD9*9395";

//Server IP
//Arduino Port
const char* serverIP = "192.168.1.10";  
const uint16_t serverPort = 5001;       

uint8_t limit = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Connection to WiFi ...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi. IP: " + WiFi.localIP().toString());

  // Connessione al server TCP
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected to TCP server");
  } else {
    Serial.println("Failed to connect to server");
  }
}

void loop() {
  if (client.connected()) {
    if(limit){
      client.println("Reached final position, sending trigger");
      client.write(0b1);   
    }
    while (client.available()) {
      String risposta = client.readStringUntil('\n');
      Serial.println("Detected object start moving cart");
    }
  } else {
    Serial.println("Connessione lost. Re-trying...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Re-connected!");
    }
  }
}
