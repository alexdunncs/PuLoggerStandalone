#ifndef DEVICESETTINGS
#define DEVICESETTINGS

#include "EEPROMAnything.h"

struct DeviceSettings {
  byte deviceID;

  DeviceSettings() : deviceID(0){}
  DeviceSettings(int deviceID) : deviceID(byte(deviceID)) {}

  void read() {EEPROM_readAnything(0, *this);}
  void write() {EEPROM_writeAnything(0, *this);}

  
};

#endif
