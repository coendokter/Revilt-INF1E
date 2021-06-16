#include <Particle.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

int fsrAnalogePin = 0; // FSR is verbonden met analoog 0
int fsrWaarde = 0; // De analoge waarde van de fsr spanningdeler

void setup() {
 
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback); 
  wifiManager.setSaveConfigCallback(saveConfigCallback); 
  wifiManager.autoConnect(); 
  delay(5000);
 
}
 
void loop() {
fsrWaarde = analogRead(fsrAnalogePin);
Serial.print("Analoge waarde = "); // print 'Analoge waarde'
Serial.println(fsrWaarde); // print de fsrwaarde op de monitor
delay(200);
}