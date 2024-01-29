#include <Wire.h>
// Include LM75 Library
#include <LM75.h>

// Define the update rate of the display and the LED
#define UPDATE_INTERVAL 500
// Baude rate of the serial interface
#define SERIAL_BAUD 115200

// LED Definitions
#define LED_PIN 13
unsigned char _pinState = 0x00;

// LM75 Variables
LM75 Lm75(LM75::LM75_Address::A0);

// Numeric counter
unsigned long _counter = 0;
// Storage for Runtime measuring for interval control
unsigned long _millisPrevious;

void setup() {
    // Init Serial interface
    Serial.begin(SERIAL_BAUD);
    // Initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_PIN, OUTPUT);
    // Init LCD Display
    Serial.println("Init LM75....");
    Lm75.begin();
}

void loop() {
    // Everytime the runtime exeeds the update intervll, we update the display
    if ((millis() - _millisPrevious) > UPDATE_INTERVAL) {
        // Write LED Pin
        digitalWrite(LED_PIN, _pinState);
        // Toggle LED Pin state
        _pinState = (_pinState == 0) ? 0x01 : 0x00;

        // Store actull time counter
        _millisPrevious = millis();
        // Reset the counter if reaches 1000
        if (_counter >= 1000) _counter = 0;
    }
}
