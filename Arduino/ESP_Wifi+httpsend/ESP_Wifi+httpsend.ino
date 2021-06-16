#include "WiFi.h"
#include "WebServer.h"
#include "DNSServer.h"
#include "ESPAsyncWebServer.h"
#include "ESPAsyncWiFiManager.h"
#include "HTTPClient.h"
#include "EEPROM.h"
#define EEPROM_SIZE 1

AsyncWebServer server(80);
DNSServer dns;

int analogePinDrukSensor = 0;
int drukWaarde = 0; 
String Server = "https://revilt.serverict.nl/ReviltApi/api/Send_ESP_Data.php?Vilt_id=1&Gewicht_glas=";
String Uri = "&user=geert&pass=geert";
const char* ViltName = "Revilt69";

void setup() {
  Serial.begin(115200);                     // initiate Serial monitor
  EEPROM.begin(EEPROM_SIZE);                // Starts library to safe bytes
  if (EEPROM.read(0) == 255){               // if the value on spot 0 is equal to 255, we reset it to the value we need, 1
    EEPROM.write(0, 1);
  }
  AsyncWiFiManager wifiManager(&server,&dns);
  if(EEPROM.read(0) == 1){                  // If the first byte of the EEPROM library is equal to 1, we will execute the following code
    wifiManager.startConfigPortal(ViltName);// Starts configportal no matter if it connected to an existing wifi
    delay(60000);                           // Gives you 60 seconds to change wifi networks if you desire
    EEPROM.write(0, 2);                     // Changes the 0 byte from 1 to 2
    EEPROM.commit();                        // Commits the change in the byte
    ESP.restart();
  }
  else if(EEPROM.read(0) == 2){            //if the 0 byte in the EEPROM library is equal to 2, we will execute the following code
    wifiManager.autoConnect(ViltName);     // This starts the config portal only if it cannot find a previously connected wifi network
    EEPROM.write(0, 1);                    // Changes the 0 byte back to 1, so the next time the ESP boots up it will give the user the 60 seconds to change wifi networks
    EEPROM.commit();                       // Commits the change in the byte
  }
  else{
    Serial.print("Mistakes were made!");   // error message if the byte is not equal to 1 or 2
    Serial.print(EEPROM.read(0));          // prints the byte on the serial monitor
  }
  wifiManager.setConfigPortalTimeout(120);
}
 
void loop() {
  if(WiFi.status() == WL_CONNECTED){      // Checks if the ESP is connected to a WiFi Network
    HTTPClient http;                      // Connects to a http Client
    drukWaarde = analogRead(analogePinDrukSensor);

    String ServerFinal = Server + drukwaarde + Uri; // Puts the api-address, values from the sensor, and the passwords in one string
    http.begin(ServerFinal.c_str());          
    int httpResponseCode = http.GET();              // Sends the information in the ServerFinal string to the API
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);               // Returns the http code received from the API
    http.end();                                     // Ends the connection with the http client
    delay(10000);
  }
}