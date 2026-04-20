# 🏠 ESP32 Alexa Smart Home Controller

A complete voice-controlled home automation system using ESP32, SinricPro IoT platform, and Amazon Alexa. Control lights, fans, and a servo-operated garage door with simple voice commands.

![ESP32](https://img.shields.io/badge/ESP32-ESP32--WROOM--32-blue)
![Alexa](https://img.shields.io/badge/Alexa-Compatible-brightgreen)
![SinricPro](https://img.shields.io/badge/SinricPro-IoT%20Platform-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## 📌 Features

| Feature | Description |
|---------|-------------|
| 🎤 **Voice Control** | Full Amazon Alexa integration via SinricPro |
| 💡 **2-Channel Switch** | Control Light and Fan independently |
| 🚗 **Garage Door Control** | Servo-operated door with smooth movement |
| 📊 **Real-time State Sync** | Device states sync between ESP32 and Alexa |
| 🔄 **Auto Reconnect** | WiFi connection recovery |
| 🛡️ **Optional Safety** | Limit switch support for door position feedback |
| ⚡ **Smooth Servo Motion** | Gradual servo movement for realistic operation |
| 📝 **State Reporting** | OPEN/CLOSED/OPENING/CLOSING door states |

---

## 🛠️ Hardware Requirements

| Component | Quantity | Purpose | Approximate Cost |
|-----------|----------|---------|------------------|
| ESP32 Development Board | 1 | Main microcontroller | $5 - $10 |
| LED (any color) | 2 | Visual indicators | $0.10 each |
| 220Ω Resistor | 2 | LED current limiting | $0.05 each |
| SG90 or MG995 Servo Motor | 1 | Garage door actuator | $2 - $10 |
| Limit Switch (Optional) | 1 | Door position detection | $1 |
| Jumper Wires | 10-15 | Connections | $2 |
| Breadboard | 1 | Prototyping | $3 |
| 5V Power Supply | 1 | Servo power (if needed) | $5 |

### For Controlling AC Appliances (Optional)
| Component | Quantity | Purpose |
|-----------|----------|---------|
| 5V Relay Module | 2 | Switch AC loads |
| AC Bulb/Fan | 1 each | Actual appliances |

---

## 🔌 Pin Connections

| ESP32 Pin | Connected To | Purpose | Notes |
|-----------|--------------|---------|-------|
| **GPIO 2** | LED 1 (+) | Light indicator | Use 220Ω resistor |
| **GPIO 18** | LED 3 / Relay IN | Fan control | Use 220Ω for LED |
| **GPIO 13** | Servo Signal (Orange/Yellow) | Garage door control | PWM output |
| **GPIO 14** | Limit Switch | Door position feedback | INPUT_PULLUP |
| **5V / VIN** | Servo VCC (Red) | Servo power | External supply recommended |
| **GND** | Servo GND (Brown), LED (-), Limit Switch | Common ground | - |

### Wiring Diagram (Text Representation)
