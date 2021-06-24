#include "WiFi.h"
#include "WebServer.h"
#include "DNSServer.h"
#include "ESPAsyncWebServer.h"
#include "ESPAsyncWiFiManager.h"
#include "HTTPClient.h"
#include "EEPROM.h"
#include "HX711.h"
#define EEPROM_SIZE 1


AsyncWebServer server(80);
DNSServer dns;
HX711 scale;


int drukWaarde; 
String Server = "https://revilt.serverict.nl/ReviltApi/api/Send_ESP_Data.php";
String Uri = "?user=geert&pass=geert&Vilt_id=1&Gewicht_glas=";
const char* ViltName = "Revilt69";
const int LOADCELL_DOUT_PIN = 26;
const int LOADCELL_SCK_PIN = 25;

void setup() {
  Serial.begin(115200);                     // initiate Serial monitor
  EEPROM.begin(EEPROM_SIZE);                // Starts library to safe bytes
  if (EEPROM.read(0) == 255 || EEPROM.read(0) == 0){               // if the value on spot 0 is equal to 255, we reset it to the value we need, 1
    EEPROM.write(0, 1);
  }
  AsyncWiFiManager wifiManager(&server,&dns);
  if(EEPROM.read(0) == 1){                  // If the first byte of the EEPROM library is equal to 1, we will execute the following code
    wifiManager.startConfigPortal(ViltName);// Starts configportal no matter if it connected to an existing wifi
    if(millis() == 20000){                  // Gives you 60 seconds to change wifi networks if you desire
      EEPROM.write(0, 2);                     // Changes the 0 byte from 1 to 2
      EEPROM.commit();                        // Commits the change in the byte
      ESP.restart();
    }
  }
  else if(EEPROM.read(0) == 2){            //if the 0 byte in the EEPROM library is equal to 2, we will execute the following code
    Serial.print("Going into autoconnect-mode");
    wifiManager.autoConnect(ViltName);     // This starts the config portal only if it cannot find a previously connected wifi network
    EEPROM.write(0, 1);                    // Changes the 0 byte back to 1, so the next time the ESP boots up it will give the user the 60 seconds to change wifi networks
    EEPROM.commit();                       // Commits the change in the byte
  }
  else{
    Serial.print("Mistakes were made!");   // error message if the byte is not equal to 1 or 2
    Serial.print(EEPROM.read(0));          // prints the byte on the serial monitor
  } 
   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}
 
void loop() {
  if(WiFi.status() == WL_CONNECTED){      // Checks if the ESP is connected to a WiFi Network
    HTTPClient http;                      // Connects to a http Client

       if (scale.is_ready()) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    drukWaarde = reading;
  } else {
    Serial.println("HX711 not found.");
  }
    String ServerFinal = Uri + drukWaarde; // Puts the api-address, values from the sensor, and the passwords in one string
    http.begin(Server);     
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(ServerFinal);     
    // int httpResponseCode = http.GET();                      // Sends the information in the ServerFinal string to the API
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);                         // Returns the http code received from the API
    Serial.println(drukWaarde);
    http.end();                                              // Ends the connection with the http client
    delay(2000);
  }
}