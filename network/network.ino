#include <ESP8266WiFi.h>
#include <String.h>

const int PORT = 5555;  /* The port to listen to connections on */
WiFiServer server(PORT);

// temporary hard coding wifi credentials for testing
const String SSID = "anWifi";
const String PASSWORD = "zwAIeyc%i#0MzQm";

//IPAddress subnet(255, 255, 255, 0);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress local_IP(192, 168, 1, 184);


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

//  if (WiFi.config(local_IP, gateway, subnet)) {
//    Serial.println("IP configured");
//  } else {
//    Serial.println("IP config failed");
//  }
  
  Serial.print("Connecting to ");
  Serial.print(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    // temporary for testing feedback
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected");
  
  Serial.print("IP address = ");
  Serial.println(WiFi.localIP().toString());

  server.begin();
  server.setNoDelay(true);  // make sure the server does not wait to collect data before sending

  Serial.print("Server started and listening on: ");
  Serial.print(WiFi.localIP().toString());
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
