/***************************************************************
 * ESP32 + Alexa (SinricPro) - Controls 2 LEDs + Garage Door
 ***************************************************************/

#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <SinricProGarageDoor.h>
#include <ESP32Servo.h>

// ================== WiFi Credentials ==================
const char* ssid = "Realme_8";
const char* password = "shreeram";

// ================== SinricPro Credentials ==================
#define APP_KEY           "7010cfc3-3a28-4268-9117-ad9b644d7f0f"
#define APP_SECRET        "d68d8075-d3d3-48bf-92b8-cd15888dd687-544afa5e-4716-4602-8195-91bfc461ffa9"

// Device IDs
#define DEVICE_ID_LED1    "68dd0216acd5d3d66b499363"    // LED 1
#define DEVICE_ID_FAN     "699356d86830d6aa38b2bfab"    // BEDROOM FAN (LED3)
#define GARAGE_DOOR_ID    "6996d69b17b32c0941bf5602"    // Garage Door

// ================== Pin Definitions ==================
#define LED_PIN1          2     // LED 1
#define LED_PIN3          18    // LED 3 (Fan)
#define SERVO_PIN         13    // Servo for garage door
#define LIMIT_SWITCH_PIN  14    // Optional limit switch

// ================== Servo Variables ==================
Servo myServo;
int currentAngle = 0;

// Garage door states
const String GARAGE_DOOR_OPEN = "OPEN";
const String GARAGE_DOOR_CLOSED = "CLOSED";
const String GARAGE_DOOR_OPENING = "OPENING";
const String GARAGE_DOOR_CLOSING = "CLOSING";
String currentDoorState = GARAGE_DOOR_CLOSED;

// ================== LED Callback Functions ==================
bool onPowerStateLED1(const String &deviceId, bool &state) {
  Serial.printf("LED1 %s turned %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");
  digitalWrite(LED_PIN1, state ? HIGH : LOW);
  return true;
}

bool onPowerStateFan(const String &deviceId, bool &state) {
  Serial.printf("Fan (LED3) %s turned %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");
  digitalWrite(LED_PIN3, state ? HIGH : LOW);
  return true;
}

// ================== Servo Movement Function ==================
void moveServoTo(int targetAngle, int speedDelay = 15) {
  Serial.printf("Moving servo: %d° -> %d°\r\n", currentAngle, targetAngle);
  
  if (targetAngle > currentAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      myServo.write(angle);
      delay(speedDelay);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      myServo.write(angle);
      delay(speedDelay);
    }
  }
  currentAngle = targetAngle;
  Serial.println("✅ Servo movement complete");
}

// ================== Update Garage Door State ==================
void updateDoorState(String state) {
  currentDoorState = state;
  SinricProGarageDoor &myGarageDoor = SinricPro[GARAGE_DOOR_ID];
  myGarageDoor.sendDoorStateEvent(state);
  Serial.println("📤 Door state sent to cloud: " + state);
}

// ================== Garage Door Callback ==================
bool onGarageDoorState(const String &deviceId, bool &state) {
  Serial.printf("📱 Garage door command: %s\r\n", state ? "OPEN" : "CLOSE");
  
  if (state) {  // Open door
    if (currentDoorState != GARAGE_DOOR_OPEN) {
      updateDoorState(GARAGE_DOOR_OPENING);
      moveServoTo(180);  // Open position
      updateDoorState(GARAGE_DOOR_OPEN);
    }
  } else {  // Close door
    if (currentDoorState != GARAGE_DOOR_CLOSED) {
      updateDoorState(GARAGE_DOOR_CLOSING);
      moveServoTo(0);   // Closed position
      updateDoorState(GARAGE_DOOR_CLOSED);
    }
  }
  return true;
}

// ================== WiFi Connection ==================
void connectWiFi() {
  Serial.printf("[WiFi] Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
  Serial.printf("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
}

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  Serial.println("\n\n🏠 ESP32 Alexa Control Starting...");
  Serial.println("- 2 LEDs (Light & Fan)");
  Serial.println("- 1 Garage Door (Servo)");
  
  // ===== Configure LED pins =====
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN3, LOW);
  
  // ===== Configure Servo =====
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(100);
  myServo.attach(SERVO_PIN, 500, 2500);
  myServo.write(0);  // Start closed
  currentAngle = 0;
  
  // ===== Configure limit switch (optional) =====
  if (LIMIT_SWITCH_PIN > 0) {
    pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  }
  
  // ===== Connect to WiFi =====
  connectWiFi();
  
  // ===== Setup SinricPro devices =====
  
  // LED 1 (Switch)
  SinricProSwitch &mySwitch1 = SinricPro[DEVICE_ID_LED1];
  mySwitch1.onPowerState(onPowerStateLED1);
  
  // Fan / LED 3 (Switch)
  SinricProSwitch &mySwitch3 = SinricPro[DEVICE_ID_FAN];
  mySwitch3.onPowerState(onPowerStateFan);
  
  // Garage Door
  SinricProGarageDoor &myGarageDoor = SinricPro[GARAGE_DOOR_ID];
  myGarageDoor.onDoorState(onGarageDoorState);
  myGarageDoor.sendDoorStateEvent(GARAGE_DOOR_CLOSED);
  
  // ===== Start SinricPro =====
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
  
  Serial.println("\n✅ Setup complete!");
  Serial.println("📱 Alexa commands:");
  Serial.println("  - 'Alexa, turn on/off Light'");
  Serial.println("  - 'Alexa, turn on/off Fan'");
  Serial.println("  - 'Alexa, open/close the garage door'");
}

// ================== Loop ==================
void loop() {
  SinricPro.handle();
  
  // ===== Optional: Check limit switch for garage door =====
  if (LIMIT_SWITCH_PIN > 0) {
    static bool lastSwitchState = HIGH;
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50;
    
    bool switchState = digitalRead(LIMIT_SWITCH_PIN);
    
    if (switchState != lastSwitchState) {
      lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (switchState == LOW) {  // Limit switch pressed
        if (currentDoorState == GARAGE_DOOR_OPENING) {
          updateDoorState(GARAGE_DOOR_OPEN);
          myServo.write(180);  // Hold at open
        } else if (currentDoorState == GARAGE_DOOR_CLOSING) {
          updateDoorState(GARAGE_DOOR_CLOSED);
          myServo.write(0);   // Hold at closed
        }
      }
    }
    lastSwitchState = switchState;
  }
  
  delay(10);
}