#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Pins
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define SS_PIN    5
#define RST_PIN   14
#define DIO0_PIN  26
#define RXD2      16
#define TXD2      17

// Objects
TinyGPSPlus gps;
HardwareSerial neogps(2);

void setup() {
  Serial.begin(115200);
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    while (1);
  }
   
  // LoRa Configuration for Range
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
}

void loop() {
  // Read GPS Data
  while (neogps.available() > 0) {
    gps.encode(neogps.read());
  }

  // Transmit every 5s if valid
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    if (gps.location.isValid()) {
      String packet = "LOC:";
      packet += String(gps.location.lat(), 6);
      packet += ",";
      packet += String(gps.location.lng(), 6);

      LoRa.beginPacket();
      LoRa.print(packet);
      LoRa.endPacket();
       
      Serial.println("Sent: " + packet);
    } else {
      Serial.println("GPS Searching...");
    }
    lastSend = millis();
  }
}