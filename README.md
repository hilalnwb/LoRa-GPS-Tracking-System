# LoRa-GPS-Tracking-System

### Project Overview
This project presents the design and implementation of a long-range, off-grid GPS tracking system utilizing LoRa modulation technology. Conventional GPS trackers depend heavily on SIM cards and cellular subscriptions. In remote areas or during disaster scenarios where cellular towers fail, these devices become useless. There is a critical need for a communication system capable of transmitting location data over long distances without relying on third-party infrastructure. 

This system operates independently of existing telecommunications networks, making it ideal for remote monitoring and search-and-rescue operations. The project focuses strictly on Point-to-Point communication, functioning as a direct device-to-device data transfer system without utilizing a LoRaWAN gateway or cloud storage.

### Hardware Components and System Architecture
The architecture is divided into two distinct modules:

* **The Transmitter Node:** Relies on an ESP32 microcontroller paired with a Ublox NEO-6M GPS module to acquire satellite data. It parses NMEA strings, encodes the data into a LoRa packet, and transmits it.
* **The Receiver Node:** Acts as a handheld unit, featuring an ESP32 and an SSD1306 OLED display. It actively listens for incoming packets, decodes the data, and updates the display with real-time coordinates.

Communication between the two nodes is established using SX1278 (Ra-02) transceiver modules operating at 433 MHz. LoRa offers a unique trade-off of very long-range transmission with low power consumption, perfect for sending small packets of data like GPS coordinates.

### Schematics and Wiring
Resolving pin conflicts on the ESP32 was a critical part of the design process. Specific hardware buses were utilized to ensure stability. For the receiver, standard I2C pins were causing layout conflicts, requiring a re-mapping of the I2C bus to GPIO 32 and 33.

### Transmitter Node Wiring

| Component | ESP32 Pin | Protocol | Function |
| :--- | :--- | :--- | :--- |
| GPS TX | GPIO 16 | UART2 | Hardware Serial RX |
| GPS RX | GPIO 17 | UART2 | Hardware Serial TX |
| LoRa MOSI | GPIO 23 | VSPI | SPI Data In |
| LoRa MISO | GPIO 19 | VSPI | SPI Data Out |
| LoRa SCK | GPIO 18 | VSPI | SPI Clock |
| LoRa NSS | GPIO 5 | VSPI | Chip Select |

### Receiver Node Wiring

| Component | ESP32 Pin | Protocol | Function |
| :--- | :--- | :--- | :--- |
| OLED SDA | GPIO 33 | I2C | Custom Data Pin |
| OLED SCL | GPIO 32 | I2C | Custom Clock Pin |
| LoRa Module | Same as Tx | SPI | Standard VSPI Bus |

### Implementation Phases

**1. Prototyping Phase**
The initial phase involved setting up the circuit on breadboards. This allowed for testing individual modules and verifying the software logic without soldering. This stage was crucial for debugging the LoRa SPI communication and confirming the GPS satellite lock in an outdoor environment.

<div align="center">
  <img src="https://github.com/user-attachments/assets/c9eb7932-cbe8-4c9f-a954-50d0bc080c88" alt="Initial Prototyping on Breadboard" width="500">
</div>

**2. PCB Design Considerations**
To conceptualize a professional and compact form factor, custom Printed Circuit Boards were designed for both nodes. The layouts optimized trace widths for power lines and ensured ground plane continuity for RF stability. Due to fabrication time constraints, the project transitioned to a perfboard implementation.

<div align="center">
  <img src="https://github.com/user-attachments/assets/8ac6756a-e36a-4bae-86aa-714419ea58e7" alt="Transmitter PCB Layout" width="400">
  <img src="https://github.com/user-attachments/assets/9d672d26-109e-4426-83e8-6bc98e3b8cc5" alt="Receiver PCB Layout" width="400">
</div>

**3. Final Fabrication**
The final prototypes were transferred to permanent perfboards to ensure mechanical stability during field testing.
* **Power Distribution:** A header block technique was used, creating a bridged solder rail on the bottom of the board to distribute 3.3V and GND.
* **RF Interference Mitigation:** On the Transmitter node, the GPS ceramic antenna was positioned facing upwards, maintaining a 3cm physical separation from the LoRa coil antenna.

<div align="center">
  <img src="https://github.com/user-attachments/assets/70dcca1a-0a0b-45b1-85f4-8d8a3c2cb300" alt="The Transmitter Node with GPS" width="300">
  <img src="https://github.com/user-attachments/assets/c34a56bc-06ef-4779-b296-22701a811ab4" alt="The Receiver Node with OLED" width="300">
</div>

### Software Logic
The firmware was developed in C++ using the Arduino IDE. The system relies on a broadcast architecture where the Transmitter blindly sends data, and the Receiver listens for a specific packet header.

**Transmitter Logic:**
* Operates on a continuous loop, parsing raw NMEA sentences using the `TinyGPS++` library.
* Validates the data to prevent sending empty coordinates before a satellite lock is achieved.
* Formats valid data into a structured string using a "LOC:" prefix as a sync identifier, then transmits it via the SX1278 module.

**Receiver Logic:**
* Constantly monitors the LoRa radio for incoming energy.
* Waits for a non-zero packet size, reads the incoming string, and checks for the "LOC:" header.
* Splits the string to separate the Latitude and Longitude if a match is found.
* Renders the data to the OLED screen along with the Received Signal Strength Indicator (RSSI).

### Results and Verification
Field testing was conducted to ensure functional reliability in an outdoor environment. 

* **GPS Lock:** The NEO-6M successfully achieved a satellite lock and transmitted the coordinates.
* **Receiver Output:** The Receiver successfully decoded the packet and displayed "TRACKER FOUND" alongside the live coordinates and an RSSI of -90 dBm.

Given the LoRa sensitivity limit of -148 dBm, this strong link margin indicates the system is capable of significantly longer ranges in Line-of-Sight conditions. The received coordinates (Latitude: 33.651144, Longitude: 73.022222) were verified against Google Maps, accurately pinpointing the testing location.

<div align="center">
  <img src="https://github.com/user-attachments/assets/e46e2bf2-f7eb-41fe-838e-2793b4e36922" alt="OLED Display showing live coordinates and Signal Strength" height="350">
  <img src="https://github.com/user-attachments/assets/94240bb8-1abb-4995-b38d-ab174aee4f9a" alt="Map Verification of Coordinates" height="350">
</div>

### Future Enhancements
Future iterations of this project will focus on the following upgrades:
* Integration of a Lithium-Polymer (LiPo) battery and a dedicated charging circuit to improve portability.
* Development of a 3D-printed enclosure to provide weatherproofing for outdoor deployments.
* Incorporation of an SD Card module to log path data for post-mission analysis.
