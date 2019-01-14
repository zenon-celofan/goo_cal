#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>


const char WiFiAPPSK[] = "escaperoom";
const char AP_NameString[] = "escaperoom";  
IPAddress local_IP(13,13,13,13);
IPAddress gateway(13,1,1,1);
IPAddress subnet(255,0,0,0);
WiFiServer server(80);
WiFiClient client;


#define SS_PIN    D4
#define RST_PIN   D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;


#define LED_PIN   D0
int led_status = 0;



void setup() {
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   //turn_off LED
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_NameString, WiFiAPPSK);

  server.begin();
}



void loop() {
  client = server.available();
  if (client) {
    web_client_handler();
  }

  if (mfrc522.PICC_IsNewCardPresent()) {
    rfid_handler();
  }

}



void rfid_handler(void) {
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();  
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
  // If we're setting the LED, print out a message saying we did
  s += "<pre>LED is now ";
  s += (led_status)?"on\n":"off\n";
  s += "</pre></html>";
  
  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

