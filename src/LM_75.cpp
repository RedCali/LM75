/*
* LM75 Temperature Sensor implementation
* Base   	: Bastian Geier
* Date      : 30.12.2023
* E-mail	: mail@bastiangeier.de
* Repo      : https://github.com/RedCali/LM75.git

* Any suggestions are welcome.
*/

#include "LM_75.h"

#pragma region PUBLIC
LM75::LM75(const uint8_t address, TwoWire *wire = &Wire) {
  _address = address;
  _wire = wire;
}

void LM75::begin() {  // set address & default pointer -> read temperature
  _config = 0b00000000;              // set config byte

  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x01);                      // sets register pointer to configuration (0x01)
  _wire->write(_config);                   // sets configuration to default
  _wire->endTransmission();
  delay(10);

  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x02);                      // sets register pointer to T-Hysteresis (0x02)
  _wire->endTransmission();
  _wire->requestFrom(_address, 2);
  if (2 <= _wire->available())  // if two bytes were received
  {
    _temperature_hyst = _wire->read();           // receive high byte (overwrites previous reading)
    _temperature_hyst = _temperature_hyst << 8;  // shift high byte to be high 8 bits
    _temperature_hyst |= _wire->read();          // receive low byte as lower 8 bits
    _temperature_hyst = _temperature_hyst >> 7;
  }
  delay(10);

  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x03);                      // sets register pointer to T-Os (0x03)
  _wire->endTransmission();
  _wire->requestFrom(_address, 2);
  if (2 <= _wire->available())  // if two bytes were received
  {
    _temperature_os = _wire->read();         // receive high byte (overwrites previous reading)
    _temperature_os = _temperature_os << 8;  // shift high byte to be high 8 bits
    _temperature_os |= _wire->read();        // receive low byte as lower 8 bits
    _temperature_os = _temperature_os >> 7;
  }
  delay(10);

  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x00);                       // sets register pointer to temperature read (0x00)
  _wire->endTransmission();
  delay(10);
}

void LM75::read(void) {  // temperature read
  _wire->requestFrom(_address, 2);
  if (2 <= _wire->available())  // if two bytes were received
  {
    _temperature_high = _wire->read();  // receive high byte (overwrites previous reading)
    _temperature_low = _wire->read();   // receive low byte as lower 8 bits
    _temperature_low = _temperature_low >> 7;
  }
}

int16_t LM75::get_i(void) {
  return ((_temperature_high * 10) + (_temperature_low * 5));
}

float LM75::get_f(void) {  // temperature read float
  return (((float)_temperature_high) + _temperature_low * 0.5);
}

void LM75::get_s(char *temp) {
  char _temperature_high_hilf = _temperature_high;

  if (_temperature_high < 0) {
    temp[0] = '-';
    _temperature_high_hilf *= -1;
    _temperature_high_hilf -= 1;
  } else {
    temp[0] = (_temperature_high_hilf / 100) % 10 + '0';
    if (temp[0] == '0')
      temp[0] = ' ';
  }
  temp[1] = (_temperature_high_hilf / 10) % 10 + '0';
  temp[2] = _temperature_high_hilf % 10 + '0';

  temp[3] = ',';

  if (_temperature_low)
    temp[4] = '5';
  else
    temp[4] = '0';
}

void LM75::config_w(uint8_t conf) {  // configuration write
  switch (conf) {
    case 0: {
      _config &= ~(1 << 0);  // delete bit_0 --> Power ON
    } break;
    case 1: {
      _config |= (1 << 0);  // set bit_0 --> Power OFF / Shout Down Mode
    } break;
    case 2: {
      _config &= ~(1 << 1);  // delete bit_1 --> Compare mode
    } break;
    case 3: {
      _config |= (1 << 1);  // set bit_1 --> Interrupt mode
    } break;
    case 4: {
      _config &= ~(1 << 2);  // delete bit_2 --> O.S. Polarity -> LOW
    } break;
    case 5: {
      _config |= (1 << 2);  // set bit_2 --> O.S. Polarity -> HIGH
    } break;
    case 6: {
      _config &= ~(1 << 3);  // delete bit_3 --> Number of faults = 1
      _config &= ~(1 << 4);  // delete bit_4
    } break;
    case 7: {
      _config |= (1 << 3);   // set bit_3 --> Number of faults = 2
      _config &= ~(1 << 4);  // delete bit_4
    } break;
    case 8: {
      _config &= ~(1 << 3);  // delete bit_3 --> Number of faults = 4
      _config |= (1 << 4);   // set bit_4
    } break;
    case 9: {
      _config |= (1 << 3);  // set bit_3 --> Number of faults = 6
      _config |= (1 << 4);  // set bit_4
    } break;
    default: {
    } break;
  }
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x01);                       // sets register pointer to _configuration (0x01)
  _wire->write(_config);                    // sets configuration to default
  _wire->endTransmission();
  delay(10);
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x00);                       // sets register pointer to temperature read (0x00)
  _wire->endTransmission();
}

uint8_t LM75::config_r(void) {  // configuration read
  return _config;
}

void LM75::thy_w(uint16_t &thy) {  // temperature hysteresis write
  _temperature_hyst = thy;
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x02);                       // sets register pointer to T-Hysteresis (0x02)
  _wire->write(_temperature_hyst);          // sets configuration to default
  _wire->endTransmission();
  delay(10);
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x00);                       // sets register pointer to temperature read (0x00)
  _wire->endTransmission();
}

uint16_t LM75::thy_r(void) {  // temperature hysteresis read
  return _temperature_hyst;
}

void LM75::tos_w(uint16_t &tos) {  // over temperature shut down write
  _temperature_os = tos;
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x03);                       // sets register pointer to T-OS (0x03)
  _wire->write(_temperature_os);            // sets configuration to default
  _wire->endTransmission();
  delay(10);
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(0x00);                       // sets register pointer to temperature read (0x00)
  _wire->endTransmission();
}

uint16_t LM75::tos_r(void) {  // over temperature shut down read
  return _temperature_os;
}
#pragma endregion PUBLIC

#pragma region PRIVATE
void LM75::temp_rs(uint8_t rs) {            // register select
  _wire->beginTransmission(_address);  // transmit to device address
  _wire->write(rs % 4);                     // sets register pointer to #rs%4 --> just 0-3
  _wire->endTransmission();
}
#pragma endregion PRIVATE