/*
 * 
 *  I accidentaly lost the version I sent to my ESP8266, this code is close to what works but hasn't been tested, don't hesitate to fork it
 * 
 */


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

WiFiClient client;

ESP8266WiFiMulti WiFiMulti;

  const uint16_t port = 80;
  const char * host = "192.168.1.1"; // ip or dns

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SSID", "passpasspass");

  while (WiFiMulti.run() != WL_CONNECTED) {
    //Serial.print(".");
    delay(200);
  }
  client.connect(host,port);
  Serial.println("OK");
  
  delay(200);
}


void loop() {


  if (!client.connected()) {
    client.connect(host,port);
    delay(5000); //if not connected waiting 5secs
  }
  else //we are connected to the server
  {
    if(Serial.available()) //we received char from serial
    {
        client.println("a");
        
        //flushing serial
        while(Serial.available())
        {
          Serial.read();
        }

        
        while(!client.available())
        {
          delay(5); //waiting for server to respond
        }

        Serial.print("GO");
        while(client.available()) //copying input for server to output (Serial)
        {
          Serial.write(client.read());
        }
        Serial.println("");
    }
  }
}
