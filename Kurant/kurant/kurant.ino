#include <ESP8266WiFi.h>
#include <EEPROM.h>
//#include <SPI.h>
//#include <MFRC522.h>

#define SETUP_COMPLETE_ADDR   0
#define WIFI_SSID_ADDR        1
#define WIFI_PASS_ADDR        34

const char* ssid = "time"; //max lenght = 32
const char* password = "whatisthetime"; //max length = 64


const char WiFiAPPSK[] = "kurant1234";
const char AP_NameString[] = "kurant1234";  
IPAddress local_IP(1,2,3,4);
IPAddress gateway(1,2,3,4);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);
WiFiClient client;


#define LED_PIN   D0
int led_status = 0;

#define BUTTON_PIN  D5

byte setup_complete;


void setup() {
  Serial.begin(9600);
  Serial.println("\n\n   ---   INIT   ---\n");
  
  EEPROM.begin(512);

  EEPROM.get(SETUP_COMPLETE_ADDR,setup_complete);
  Serial.print("Configuration status: ");
  Serial.println(setup_complete);

  if (setup_complete == 0) {
    EEPROM.get(WIFI_SSID_ADDR, ssid);
    EEPROM.get(WIFI_PASS_ADDR, password);
    Serial.println(ssid);
    Serial.println(password);
  }

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   //turn_off LED

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_NameString, WiFiAPPSK);

  server.begin();

  Serial.println("   ---   Setup complete   ---");
}



void loop() {

  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Resetting...);
    EEPROM.write(0, 0xFF);
    EEPROM.commit();
    sw_reset();
  }
    
  client = server.available();
  if (client) {
    web_client_handler();
  }

  delay(100);
}


void web_client_handler(void) {
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  if (req.indexOf("name=") != -1)
    ssid = "";
    while (char n != '&') {
      ssid += req
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
  s += "WiFi name: <input type=\"text\" name=\"WiFiName\" value=\"wifi_name\">\n";
  s += "WiFi pass: <input type=\"text\" pass=\"WiFiPass\" value=\"wifi_password\">\n";
  s += "<input type=\"submit\" value=\"Submit\">\n";
  s += "</form>";
  s += "</pre></html>";
 
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

void(* sw_reset) (void) = 0; //declare reset function @ address 0
