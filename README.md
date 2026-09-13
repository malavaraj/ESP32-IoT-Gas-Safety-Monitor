# ESP32 IoT Gas Safety Monitor

An IoT-based gas leakage monitoring and automatic exhaust fan control system using ESP32, a gas sensor, relay module, buzzer, and a real-time web dashboard.

## Project Overview

This project is designed to detect gas leakage and provide an automatic safety response.

The ESP32 continuously reads the gas sensor value. When the gas level exceeds the predefined threshold, the system automatically activates the exhaust fan and buzzer.

A web-based dashboard allows the user to monitor the gas level, gas status, exhaust fan status, and buzzer status in real time.

## Features

- Real-time gas leakage detection
- ESP32-based IoT monitoring
- Automatic exhaust fan control
- Automatic buzzer alert
- Real-time web dashboard
- Gas sensor value monitoring
- Manual exhaust fan control
- Safety priority during gas leakage
- Wi-Fi connectivity
- Serial Monitor status monitoring

## System Working

### Safe Condition

When the gas sensor value is below the threshold:

- Gas Status: SAFE
- Exhaust Fan: OFF
- Buzzer: OFF

### Gas Leakage Condition

When the gas sensor value reaches or exceeds the threshold:

- Gas Status: GAS LEAKAGE
- Exhaust Fan: ON
- Buzzer: ON

The fan is automatically activated during gas leakage.

## Hardware Components

| Component | Quantity |
|---|---:|
| ESP32 Development Board | 1 |
| MQ-Series Gas Sensor | 1 |
| 1-Channel Relay Module | 1 |
| DC Exhaust Fan | 1 |
| Buzzer | 1 |
| Breadboard | 1 |
| Connecting Wires | As required |
| Power Supply | 1 |

## Pin Configuration

| Component | ESP32 Pin |
|---|---|
| Gas Sensor Analog Output | GPIO 34 |
| Relay Module IN | GPIO 26 |
| Buzzer | GPIO 27 |

## Gas Detection

The gas sensor is connected to the ESP32 ADC through GPIO 34.

The configured gas threshold is:

```cpp
#define GAS_THRESHOLD 500
The system operates according to the following logic:

Gas Value < 500
       |
       v
     SAFE
       |
       +---- Fan OFF
       |
       +---- Buzzer OFF
Gas Value >= 500
       |
       v
 GAS LEAKAGE
       |
       +---- Fan ON
       |
       +---- Buzzer ON
Relay Control

The relay is used to control the exhaust fan.

The relay control pin is:

GPIO 26

The intended control logic is:

GPIO HIGH → Relay ON → Fan ON
GPIO LOW  → Relay OFF → Fan OFF
Web Dashboard

The ESP32 hosts a web server over Wi-Fi.

The dashboard displays:

Gas Status
Sensor Value
Exhaust Fan Status
Buzzer Status
Gas Limit
Fan ON/OFF controls
Dashboard

Serial Monitor

The ESP32 provides real-time information through the Arduino Serial Monitor.

Example output:

Gas: 294 | Status: SAFE | Fan: OFF | Buzzer: OFF
Gas: 282 | Status: SAFE | Fan: OFF | Buzzer: OFF
Gas: 576 | Status: GAS LEAKAGE | Fan: ON | Buzzer: ON
Gas: 655 | Status: GAS LEAKAGE | Fan: ON | Buzzer: ON
Serial Monitor

Software and Technologies
ESP32
Arduino IDE
C/C++
HTML
CSS
JavaScript
Wi-Fi
Embedded Systems
IoT
Web Server
Libraries
#include <WiFi.h>
#include <WebServer.h>
Installation
1. Open the Project

Open gas_safety_monitor.ino using Arduino IDE.

2. Configure Wi-Fi

Replace the following with your own Wi-Fi credentials:

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
3. Select the ESP32 Board

In Arduino IDE:

Tools → Board → ESP32 Arduino → ESP32 Dev Module
4. Select the COM Port

Connect the ESP32 to your computer and select the appropriate COM port.

5. Upload the Program

Upload the program to the ESP32.

6. Open Serial Monitor

Set the baud rate to:

115200

The ESP32 will display its IP address after connecting to Wi-Fi.

Example:

WIFI CONNECTED!
IP Address: ESP32_IP_ADDRESS
Web Server Started!
7. Open the Web Dashboard

Enter the ESP32 IP address shown in the Serial Monitor into a web browser.

http://ESP32_IP_ADDRESS

The computer or mobile device should be connected to the same Wi-Fi network as the ESP32.

Project Structure
ESP32-IoT-Gas-Safety-Monitor/
│
├── images/
│   ├── web-dashboard.jpg
│   └── serial-monitor.jpg
│
├── gas_safety_monitor.ino
├── README.md
└── LICENSE
Future Improvements
Gas sensor calibration
Cloud data logging
Historical gas-level graphs
Mobile notifications
Mobile application
OLED/LCD display
Multiple gas sensors
Emergency notification system
Battery backup
Improved gas concentration estimation
Safety Note

This project is an educational prototype.

For demonstration, a low-voltage DC exhaust fan should be used.

Do not connect mains-voltage equipment directly to a breadboard or prototype circuit. Real-world deployment requires appropriate electrical isolation, protection, enclosure, and qualified electrical installation.
