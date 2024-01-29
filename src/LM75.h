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
#include <Wire.h>S
#include <avr/io.h>

#include "Arduino.h"

#else
#pragma message("Compiling for AVR ATMEGA Framework Architecture...")

#include <avr/io.h>
#endif

#include <stdint.h>

#pragma region Constant definition
// LM75 Commandset
#define ON 0
#define SHUTDOWN 1
#define OFF 1

#define COMP 2
#define INT 3

#define OS_POL_LOW 4
#define OS_POL_HIGH 5

#define FAULT_QUEUE_1 6
#define FAULT_QUEUE_2 7
#define FAULT_QUEUE_4 8
#define FAULT_QUEUE_6 9
#pragma endregion Constant definition

class LM75 {
   public:
#pragma region PUBLIC
    explicit LM75(const uint8_t deviceAddress, TwoWire *wire = &Wire);

    void begin();            // Set address & default pointer -> read temperature
    void read(void);         // Temperature read
    int16_t get_i(void);     // Temperature read
    float get_f(void);       // Temperature read
    void get_s(char *temp);  // Temperature read

    void config_w(uint8_t conf);      // Configuration write
    uint8_t config_r(void);           // Configuration read
    void thy_w(const uint16_t &thy);  // Temperature hysteresis write
    uint16_t thy_r(void);             // Temperature hysteresis read
    void tos_w(const uint16_t &tos);  // Temperature over temperature shut down write
    uint16_t tos_r(void);             // Temperature over temperature shut down read
#pragma endregion PUBLIC

   private:
#pragma region PRIVATE
    TwoWire *_wire;
    uint8_t _address = 0;
    uint8_t _config = 0;
    int16_t _temperature_hyst = 0;
    int16_t _temperature_os = 0;
    uint8_t _temperature_high = 0;
    uint8_t _temperature_low = 0;
    void temp_rs(uint8_t rs);  // Register select
#pragma endregion PRIVATE
};

#endif /* LM75_H_ */
