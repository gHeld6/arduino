#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x88, 0x64
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 71);
IPAddress gateway(192, 168, 1, 264);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server(80);

String readString;
const int LED = 2;

void setup() {
  pinMode(LED, OUTPUT);
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // initialize the Ethernet device not using DHCP:
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // start listening for clients
  server.begin();
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    while(client.connected()){
      if(client.available()){
        char c = client.read();
        if(readString.length() < 100){
          readString += c;
        }
        if(c == 13){
          Serial.println(readString);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Ethernet</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h2><a href=\"/?ON\">ON</a>");
          client.println("<h2><a href=\"/?OFF\">OFF</a>");
          client.println("</body>");
          client.println("<html>");

          delay(10);
          client.stop();
          if(readString.indexOf("?ON") > -1){
            digitalWrite(LED, HIGH);
          } else if(readString.indexOf("?OFF") > -1){
            digitalWrite(LED, LOW);
          }
          readString = "";
        }
      }
    }
    /*if (!gotAMessage) {
      Serial.println("We have a new client");
      client.println("Hello, client!");
      gotAMessage = true;
    }

    // read the bytes incoming from the client:
    char thisChar = client.read();
    // echo the bytes back to the client:
    server.write(thisChar);
    // echo the bytes to the server as well:
    Serial.print(thisChar);
    Ethernet.maintain();*/
  }
}
