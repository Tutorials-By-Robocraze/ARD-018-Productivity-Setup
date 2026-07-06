#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const int FSR_DIGITAL_PIN = 2; // Digital input pin
const int BUZZER_PIN = 11;     // PWM pin for buzzer volume control
const int BUZZER_VOLUME = 200;  

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 200; 

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Changed from INPUT_PULLUP to INPUT because we are using an external 10k resistor
  pinMode(FSR_DIGITAL_PIN, INPUT); 
  
  Serial.begin(115200);
  u8g2.begin(); 
  
  Serial.println(F("\n=== EXTERNAL SCHMITT-TRIGGER BYPASS ACTIVE ==="));
}

void loop() {
  // Read digital state: HIGH (1) = Empty, LOW (0) = Phone Present
  int pinState = digitalRead(FSR_DIGITAL_PIN);
  int phonePresent = (pinState == LOW); 
  
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = currentTime;

    // Output live telemetry to monitor
    Serial.print(F("Pin D2 State: "));
    Serial.print(pinState);
    Serial.print(F(" | Focus Status: "));
    Serial.println(phonePresent ? "FOCUS TIME" : "PHONE MISSING");

    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB12_tr); 

      if (phonePresent) {
        u8g2.drawStr(0, 28, "Focus Mode");
        u8g2.drawStr(0, 48, "Activated.");
      } 
      else {
        u8g2.drawStr(0, 28, "Put the");
        u8g2.drawStr(0, 48, "phone down!");
      }
    } while ( u8g2.nextPage() );
  }

  // Alarm Control Logic
  if (phonePresent) {
    analogWrite(BUZZER_PIN, 0); 
    delay(20); 
  } 
  else {
    analogWrite(BUZZER_PIN, BUZZER_VOLUME); 
    delay(150);
    analogWrite(BUZZER_PIN, 0); 
    delay(150);
  }
}