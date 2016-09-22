/**
 * Movimento Tracker v0.0.1
 * 
 */
 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial gps(4,3);

void setup() {
  Serial.begin(9600);

  while(!Serial);

  gps.begin(9600);
  delay(1000);

  Serial.println("Setul complete!");

}

void loop() {
  if( gps.available() ){
    Serial.write(gps.read());
  }

  if( Serial.available() ){
    gps.write(Serial.read());
  }

}
