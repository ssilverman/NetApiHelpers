/*
 Advanced WiFi Chat Server - server.accept() demo/test

 A more advanced server that distributes any incoming messages
 to all connected clients but the client the message comes from.
 To use, telnet to your device's IP address and type.
 You can see the client's input in the serial monitor as well.

 */

#include <WiFi.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

// telnet defaults to port 23
WiFiServer server(23);

WiFiClient clients[8];

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print("Attempting to connect to SSID \"");
  Serial.print(ssid);
  Serial.println("\" with DHCP ...");
  int status = WiFi.begin(ssid, pass);
//  status = WiFi.waitForConnectResult();
  if (status != WL_CONNECTED) {
    Serial.println("STA didn't connect");
    while (true) {
      delay(1);
    }
  } else {
    Serial.println("\t...success");
  }
  Serial.println();

  // start the server:
  server.begin();

  Serial.print("Chat server address:");
  Serial.println(WiFi.localIP());
}

void loop() {
  // check for any new client connecting, and say hello (before any incoming data)
  WiFiClient newClient = server.accept();
  if (newClient) {
    for (byte i=0; i < 8; i++) {
      if (!clients[i]) {
        Serial.print("We have a new client #");
        Serial.println(i);
        newClient.print("Hello, client number: ");
        newClient.println(i);
        // Once we "accept", the client is no longer tracked by WiFiServer
        // so we must store it into our list of clients
        clients[i] = newClient;
        break;
      }
    }
  }

  // check for incoming data from all clients
  for (byte i=0; i < 8; i++) {
    if (clients[i] && clients[i].available() > 0) {
      // read bytes from a client
      byte buffer[80];
      int count = clients[i].read(buffer, 80);
      // write the bytes to all other connected clients
      for (byte j=0; j < 8; j++) {
        if (j != i && clients[j].connected()) {
          clients[j].write(buffer, count);
        }
      }
    }
  }

  // stop any clients which disconnect
  for (byte i=0; i < 8; i++) {
    if (clients[i] && !clients[i].connected()) {
      Serial.print("disconnect client #");
      Serial.println(i);
      clients[i].stop();
    }
  }

}
