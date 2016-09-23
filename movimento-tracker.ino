/**
 * Movimento Tracker v0.0.1
 * 
 */

// Libraries
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 433.0

// Constant
static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600;

// TGPSP object
TinyGPSPlus gps;

// The serial object to uBlox
SoftwareSerial serUblox(RXPin, TXPin);

// Radiohead LoRa
RH_RF95 rf95;


/**
 * Constructor
 */
void setup() {
  Serial.begin(115200);
  serUblox.begin(GPSBaud);

  setupLora();
    
  delay(1000);
  Serial.println("Movimento ready!");
}

void setupLora(){
  // Interrupt and reset pin
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
}


/**
 * Main Loop
 */
void loop() {
  while( serUblox.available() > 0 ){
    if( gps.encode(serUblox.read()) ){
    
      // Debug if connected to PC serial
      //if( Serial.available() > 0 ){
        debugSerial();
      //}
    }

    // Halt on GPS device
    if (millis() > 5000 && gps.charsProcessed() < 10){
      Serial.println(F("No GPS detected: check wiring."));
      while(true);
    }
    
  }

  

}

int16_t packetnum = 0;  // packet counter, we increment per xmission
void debugInfoLora(){
  String info = "Sending to rf95_server ";
  info += RH_RF95_MAX_MESSAGE_LEN;
  Serial.println(info);
  char radiopacket[20] = "Hello World #      ";
  
  if (gps.location.isValid())
  {
    String locStr = "Loc : ";
    locStr += gps.location.lat();
    
    locStr.toCharArray(radiopacket, 20);
    //itoa(gps.location.lat(), radiopacket+5, 10) ;
    //itoa(gps.location.lng(), radiopacket+12, 10);
    
  } else {
    // Send a message to Movimento gateway
    itoa(packetnum++, radiopacket+13, 10);
  }
  
  Serial.print("Sending "); 
  Serial.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  
  delay(1000);
}

/**
 * Serial debugging
 */
void debugSerial(){
  debugInfoGPS();
  debugInfoLora();
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

