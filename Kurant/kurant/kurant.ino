#include <ESP8266WiFi.h>
#include <EEPROM.h>
//#include <SPI.h>
//#include <MFRC522.h>


const char WiFiAPPSK[] = "kurant1234";
const char AP_NameString[] = "kurant1234";  
IPAddress local_IP(1,2,3,4);
IPAddress gateway(1,2,3,4);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);
WiFiClient client;


#define LED_PIN   D0
int led_status = 0;

byte setup_complete;


void setup() {
  Serial.begin(9600);
  Serial.println("\n\n   ---   INIT   ---\n");
  EEPROM.begin(1);

  EEPROM.get(0,setup_complete);
  Serial.print("Configuration status: ");
  Serial.println(setup_complete);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   //turn_off LED
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_NameString, WiFiAPPSK);

  server.begin();

  Serial.println("   ---   Setup complete   ---");
}



void loop() {
  client = server.available();
  if (client) {
    web_client_handler();
  }

}


void web_client_handler(void) {
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  if (req.indexOf("/led=0") != -1)
    led_status = 0;
  else if (req.indexOf("/led=1") != -1)
    led_status = 1; // Will write LED high

  //digitalWrite(LED_PIN, led_status);

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  s += "<pre>\n";
  s += "<form action=\"/action_page.php\">\n";
  s += "WiFi name: <input type=\"text\" name=\"WiFiName\" value=\"network name\">\n";
  s += "WiFi pass: <input type=\"text\" name=\"WiFiPass\" value=\"password\">\n";
  s += "<input type=\"submit\" value=\"Submit\">\n";
  s += "</form>";
  s += "</pre></html>";
 
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

