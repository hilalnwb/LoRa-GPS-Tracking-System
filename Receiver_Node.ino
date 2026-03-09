#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PCB Pin Definitions
#define OLED_SDA_PIN  33
#define OLED_SCL_PIN  32

// LoRa Pins
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define SS_PIN    5
#define RST_PIN   14
#define DIO0_PIN  26

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Init I2C on Custom Pins
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Init Failed"));
    for(;;);
  }
   
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  // Initialize LoRa
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
   
  if (!LoRa.begin(433E6)) {
    display.println("LoRa Error!");
    display.display();
    while (1);
  }
   
  // LoRa Config (Match Tx)
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    int rssi = LoRa.packetRssi();

    if (incoming.startsWith("LOC:")) {
      String coords = incoming.substring(4); 
      int commaIndex = coords.indexOf(',');
      String lat = coords.substring(0, commaIndex);
      String lng = coords.substring(commaIndex + 1);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("TRACKER FOUND");
      display.println("----------------");
      display.print("Lat: "); display.println(lat);
      display.print("Lng: "); display.println(lng);
      display.setCursor(0, 50);
      display.print("RSSI: "); display.print(rssi);
      display.display();
    }
  }
}