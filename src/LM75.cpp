/*
* LM75 Temperature Sensor implementation
* Base    : Bastian Geier
* Date    : 30.12.2023
* E-mail  : mail@bastiangeier.de
* Repo    : https://github.com/RedCali/LM75.git

* Any suggestions are welcome.
*/

#include "LM75.h"

#pragma region PUBLIC
LM75::LM75(LM75_Address address, TwoWire *wire = &Wire) {
    _address = address;
    _wire = wire;
}

void LM75::begin() {                     // set address & default pointer -> read temperature
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_CONFIG);  // sets register pointer to configuration (0x01)
    _wire->write(_config);               // sets configuration to default
    _wire->endTransmission();
    delay(10);

    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_HYSTER);  // sets register pointer to T-Hysteresis (0x02)
    _wire->endTransmission();
    _wire->requestFrom(_address, 2);
    if (2 <= _wire->available()) {                    // if two bytes were received
        _internals.hysteresis = _wire->read() << 1;   // receive high byte (overwrites previous reading) and shift high byte to the high 8 bits
        _internals.hysteresis |= _wire->read() >> 7;  // receive low byte as lower 8 bits and shift one to the right
    }
    delay(10);

    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_OS);      // sets register pointer to T-Os (0x03) / Over temperature ShutDown
    _wire->endTransmission();
    _wire->requestFrom(_address, 2);
    if (2 <= _wire->available()) {            // if two bytes were received
        _internals.os = _wire->read() << 1;   // receive high byte (overwrites previous reading) and  shift high byte to be high 8 bits
        _internals.os |= _wire->read() >> 7;  // receive low byte as lower 8 bits and shift one to the right
    }
    delay(10);

    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_TEMP);    // sets register pointer to temperature read (0x00)
    _wire->endTransmission();
    delay(10);
}

void LM75::read(void) {  // temperature read
    _wire->requestFrom(_address, 2);
    if (2 <= _wire->available()) {              // if two bytes were received
        _internals.temp = _wire->read() << 1;   // receive high byte (overwrites previous reading)
        _internals.temp |= _wire->read() >> 7;  // receive low byte as lower 8 bits and shift 7 to the right
    }
}

int16_t LM75::get_i(void) {
    return 0;  //((_temperature_high * 10) + (_temperature_low * 5));
}

float LM75::get_f(void) {  // temperature read float
    return 0;              //(((float)_temperature_high) + _temperature_low * 0.5);
}

void LM75::get_s(char *temp) {
    char _temperature_high_hilf = 0;  //_temperature_high;
    /*
        if (_temperature_high < 0) {
            temp[0] = '-';
            _temperature_high_hilf *= -1;
            _temperature_high_hilf -= 1;
        } else {
            temp[0] = (_temperature_high_hilf / 100) % 10 + '0';
            if (temp[0] == '0') {
                temp[0] = ' ';
            }
        }
        temp[1] = (_temperature_high_hilf / 10) % 10 + '0';
        temp[2] = _temperature_high_hilf % 10 + '0';

        temp[3] = ',';

        if (_temperature_low) {
            temp[4] = '5';
        } else {
            temp[4] = '0';
        }
        */
}

void LM75::setConfig(LM75_Configuration config) {
    _config = (LM75_Configuration)((uint8_t)_config | (uint8_t)config);
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_CONFIG);  // sets register pointer to _configuration (0x01)
    _wire->write(_config);               // sets configuration to default
    _wire->endTransmission();
    delay(10);
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_TEMP);    // sets register pointer to temperature read (0x00)
    _wire->endTransmission();
}

void LM75::setConfig(LM75_Configuration config) {
    _config = (LM75_Configuration)((uint8_t)_config & ~((uint8_t)config));
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_CONFIG);  // sets register pointer to _configuration (0x01)
    _wire->write(_config);               // sets configuration to default
    _wire->endTransmission();
    delay(10);
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_TEMP);    // sets register pointer to temperature read (0x00)
    _wire->endTransmission();
}

LM75::LM75_Configuration LM75::readConfig(void) {  // read configuration from chip
    _wire->beginTransmission(_address);            // transmit to device address
    _wire->write(LM75_REGISTER_CONFIG);            // sets register pointer to T-Hysteresis (0x02)
    _wire->endTransmission();
    _wire->requestFrom(_address, 2);
    if (1 <= _wire->available()) {                    // if two bytes were received
        _config = (LM75_Configuration)_wire->read();  // receive byte (overwrites previous reading)
    }
    return _config;
}
LM75::LM75_Configuration LM75::getConfig(void) {  // get configuration
    return _config;
}

void LM75::thy_w(const uint16_t &thy) {  // temperature hysteresis write
    _internals.hysteresis = thy;
    _wire->beginTransmission(_address);   // transmit to device address
    _wire->write(LM75_REGISTER_HYSTER);   // sets register pointer to T-Hysteresis (0x02)
    _wire->write(_internals.hysteresis);  // sets configuration to default
    _wire->endTransmission();
    delay(10);
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_TEMP);    // sets register pointer to temperature read (0x00)
    _wire->endTransmission();
}

uint16_t LM75::thy_r(void) {  // temperature hysteresis read
    return _internals.hysteresis;
}

void LM75::tos_w(const uint16_t &tos) {  // over temperature shut down write
    _internals.os = tos;
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_OS);      // sets register pointer to T-OS (0x03)
    _wire->write(_internals.os);         // sets configuration to default
    _wire->endTransmission();
    delay(10);
    _wire->beginTransmission(_address);  // transmit to device address
    _wire->write(LM75_REGISTER_TEMP);    // sets register pointer to temperature read (0x00)
    _wire->endTransmission();
}

uint16_t LM75::tos_r(void) {  // over temperature shut down read
    return _internals.os;
}
#pragma endregion PUBLIC

#pragma region PRIVATE

#pragma endregion PRIVATE