#include <ESP8266WiFi.h>
#include <String.h>

const int PORT = 5555;  /* The port to listen to connections on */
WiFiServer server(PORT);

// change these values
const String SSID = "temporary-sap-id";
const String PASSWORD = "123456789";


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD, 1, false, 1);
  Serial.print("IP address = ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  server.setNoDelay(true);  // make sure the server does not wait to collect data before sending

  Serial.print("Server started and listening on: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(PORT);
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
