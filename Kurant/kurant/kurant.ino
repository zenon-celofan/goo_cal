#include <ESP8266WiFi.h>
#include <EEPROM.h>
//#include <SPI.h>
//#include <MFRC522.h>

#define SETUP_COMPLETE_ADDR   0
#define WIFI_SSID_ADDR        1
#define WIFI_SSID_MAX_LEN     32
#define WIFI_PASS_ADDR        34
#define WIFI_PASS_MAX_LEN     64

char ssid[WIFI_SSID_MAX_LEN];
char password[WIFI_PASS_MAX_LEN];

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
const byte interruptPin = D5;

byte setup_complete;


void setup() {
  Serial.begin(9600);
  Serial.println("\n\n   ---   INIT   ---\n");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING);


  EEPROM.begin(512);

  EEPROM.get(SETUP_COMPLETE_ADDR, setup_complete);
  Serial.print("Configuration status: ");
  Serial.println(setup_complete);

  if (setup_complete == 0) {
    Serial.println("WiFi already configured:");
    for (byte a = 0, n = WIFI_SSID_ADDR; n < WIFI_SSID_ADDR + WIFI_SSID_MAX_LEN; n++) {
      ssid[a++] = EEPROM.read(n);
    }
    for (byte a = 0, n = WIFI_PASS_ADDR; n < WIFI_PASS_ADDR + WIFI_PASS_MAX_LEN; n++) {
      password[a++] = EEPROM.read(n);
    }
    Serial.println(ssid);
    Serial.println(password);
    Serial.print("Connecting to wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  } else {
    Serial.println("WiFi not configured, starting AP");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_NameString, WiFiAPPSK);

    server.begin();
  }
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   //turn_off LED

  
  Serial.println("   ---   Setup complete   ---");
}



void loop() {
   
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

  int n = req.indexOf("name=");

  if (n != -1) {
    while (req[n++] != '=');
    Serial.print("found name at:");
    Serial.println(n);
    byte a = 0;
    while (req[n] != '&')  {
      ssid[a] = req[n++];
      Serial.print(ssid[a++]);
    }
    
    Serial.println();
    
    for (byte a = 0, n = WIFI_SSID_ADDR; n < WIFI_SSID_ADDR + WIFI_SSID_MAX_LEN; n++) {
      EEPROM.write(n, ssid[a++]);
    }
    //EEPROM.put(SETUP_COMPLETE_ADDR, 0);
  }
  
  n = req.indexOf("pass=");

  if (n != -1) {
    while (req[n++] != '=');
    Serial.print("found pass at:");
    Serial.println(n);
    byte a = 0;
    while (req[n] != ' ') {
      password[a] = req[n++];
      Serial.print(password[a++]);
    }

    Serial.println();

    for (byte a = 0, n = WIFI_PASS_ADDR; n < WIFI_PASS_ADDR + WIFI_PASS_MAX_LEN; n++) {
      EEPROM.write(n, password[a++]);
    }
    EEPROM.write(SETUP_COMPLETE_ADDR, 0);

    EEPROM.commit();
    Serial.println("eeprom updated");
    
    client.flush();
    
    ESP.reset();
  }
  
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  s += "<pre>\n";
  s += "<form action=\"/\">\n";
  s += "WiFi name: <input type=\"text\" name=\"name\" value=\"wifi_name\">\n";
  s += "WiFi pass: <input type=\"text\" name=\"pass\" value=\"wifi_password\">\n";
  s += "<input type=\"submit\" value=\"Submit\">\n";
  s += "</form>";
  s += "</pre></html>";
 
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

void handleInterrupt() {
    Serial.println("Resetting...");
    EEPROM.write(SETUP_COMPLETE_ADDR, 0xFF);
    EEPROM.commit();
    ESP.restart();
}
