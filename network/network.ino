#include <ESP8266WiFi.h>
#include <String.h>

const int PORT = 5555;  /* The port to listen to connections on */
WiFiServer server(PORT);

// temporary hard coding wifi credentials for testing
const String SSID = "YOUR_WIFI_NAME";
const String PASSWORD = "YOUR_WIFI_PASSWORD";


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  
  Serial.printf("Connecting to %s ", SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    // temporary for testing feedback
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected");
  
  Serial.printf("IP address = %s\n", WiFi.localIP());

  server.begin();
  server.setNoDelay(true);  // make sure the server does not wait to collect data before sending

  Serial.printf("Server started and listening on: %s:%d\n", WiFi.localIP(), PORT);
}

void loop() {
  WiFiClient client = server.accept();  // non blocking?

  if (client) {
    Serial.println("Connection established");

    // a client has connected
    while (client.connected()) {
      if (client.available()) {
        // read a message from the client
        String message = client.readStringUntil('\r');
        Serial.println(message);

        // send back an acknowledge
        client.print("Message received");
      }
    }

    while (client.available()) {
      client.read();  // make sure the client is done talking
    }

    client.stop();
    Serial.println("Connection closed");
  }
}
