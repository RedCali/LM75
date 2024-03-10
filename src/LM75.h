/*
* LM75 Temperature Sensor implementation
* Base   	: Bastian Geier
* Date    : 30.12.2023
* E-mail	: mail@bastiangeier.de
* Repo    : https://github.com/RedCali/LM75.git

* Any suggestions are welcome.
*/
#pragma once
#ifndef LM75_H_
#define LM75_H_

#define LM75_VERSION "0.1.0"

#if defined(ARDUINO) && ARDUINO >= 100
#pragma message("Compiling for Arduino Framework Architecture...")
#include <Wire.h>
#include <avr/io.h>

#include "Arduino.h"

#else
#pragma message("Compiling for AVR ATMEGA Framework Architecture...")

#include <avr/io.h>
#endif

#include <stdint.h>

#pragma region Constant definition
// LM75 Command set
#define LM75_REGISTER_TEMP 0x01    // Temperature
#define LM75_REGISTER_CONFIG 0x01  // Configuration
#define LM75_REGISTER_HYSTER 0x02  // Temperature Hysteresis
#define LM75_REGISTER_OS 0x03      // Overtemperatur ShutDown

#define LM75_ADDRESS_BASE 0b01001000
#pragma endregion Constant definition

class LM75 {
   public:
#pragma region PUBLIC
    enum LM75_Configuration : uint8_t {
        SHUT_DOWN = 0b00000001,    // if activated, chip is in low power / power safe mode
        INTERRUPT = 0b00000010,    // if deactivated it works in comparator mode
        OS_POL_HIGH = 0b00000100,  // if deactivated it works in LOW Pol mode mode
        FAULT_Q_1 = 0b00000000,
        FAULT_Q_2 = 0b00001000,
        FAULT_Q_4 = 0b00010000,
        FAULT_Q_6 = 0b00011000,
    };
    enum LM75_Address : uint8_t {
        A0 = LM75_ADDRESS_BASE,
        A1 = A0 + 1,
        A2 = A0 + 2,
        A3 = A0 + 3,
        A4 = A0 + 4,
        A5 = A0 + 5,
        A6 = A0 + 6,
        A7 = A0 + 7,
    };

    explicit LM75(LM75_Address deviceAddress, TwoWire *wire = &Wire);

    void begin();            // Set address & default pointer -> read temperature
    void read(void);         // Temperature read
    int16_t get_i(void);     // Temperature read
    float get_f(void);       // Temperature read
    void get_s(char *temp);  // Temperature read

    void setConfig(LM75_Configuration conf);    // Set Configuration
    void resetConfig(LM75_Configuration conf);  // Set Configuration
    LM75_Configuration readConfig(void);        // Read Configuration
    LM75_Configuration getConfig(void);         // Get Configuration
    void thy_w(const uint16_t &thy);            // Temperature hysteresis write
    uint16_t thy_r(void);                       // Temperature hysteresis read
    void tos_w(const uint16_t &tos);            // Temperature over temperature shut down write
    uint16_t tos_r(void);                       // Temperature over temperature shut down read
#pragma endregion PUBLIC

   private:
#pragma region PRIVATE
    TwoWire *_wire;

    LM75_Address _address = LM75_Address::A0;
    LM75_Configuration _config = (LM75_Configuration)0x00;

    struct LM75_Internals {
        int16_t hysteresis;
        int16_t os;
        int8_t tempHigh;
        int8_t tempLow;
    } _internals = {0, 0, 0, 0};

    inline void selectRegister(uint8_t reg) {  // Register select
        _wire->beginTransmission(_address);    // Transmit to device address
        _wire->write(reg % 4);                 // Sets register pointer to #rs%4 --> just 0-3
        _wire->endTransmission();
    }
#pragma endregion PRIVATE
};

#endif /* LM75_H_ */
