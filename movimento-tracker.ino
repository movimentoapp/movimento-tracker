/**
 * Movimento Tracker v0.0.1
 * 
 */

// Libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Constant
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// TGPSP object
TinyGPSPlus gps;

// The serial object to uBlox
SoftwareSerial serUblox(RXPin, TXPin);

/**
 * Constructor
 */
void setup() {
  Serial.begin(115200);
  serUblox.begin(GPSBaud);
  
  delay(1000);
  Serial.println("Movimento ready!");
}


/**
 * Main Loop
 */
void loop() {
  while( serUblox.available() > 0 ){
    if( gps.encode(serUblox.read()) ){
    
      // Debug if connected to PC serial
      if( Serial.available() > 0 ){
        debugSerial();
      }
    }
    
    // Halt on GPS device
    if (millis() > 5000 && gps.charsProcessed() < 10){
      Serial.println(F("No GPS detected: check wiring."));
      while(true);
    }
    
  }

}


/**
 * Serial debugging
 */
void debugSerial(){
  debugInfoGPS();
}


/**
 * GPS serial debugging
 */
void debugInfoGPS(){
  Serial.print(F("Location: "));
    
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

