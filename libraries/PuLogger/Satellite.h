#ifndef SATELLITE
#define SATELLITE

#include "userSettings.h"

class Satellite { //privatise whatever when you have a final architecture figured out.
private:
	
public:
	Satellite();
	Satellite(int deviceID, bool hasAdditionalSensor, unsigned long int tGrace, int tHigh, int tLow, unsigned long int hGrace, int hHigh, int hLow);
  
	int deviceID;
	bool deviceUp;
	bool hasAdditionalSensor; //should be const, with getter
	unsigned long int lastTransmission;
	
	int tempRawValue[2]; //Raw values in deci-units.
  int humRawValue[2];
	
	int tempRawAvg;
	int humRawAvg;
	
	int tempHighLimit; // Temperature alarm properties
	int tempLowLimit;
	bool tempHighAlarm;
	bool tempLowAlarm;
	unsigned long int tempFirstOOR;
	unsigned long int tempAlarmGracePeriod;
	
	int humHighLimit; // Humidity alarm properties
	int humLowLimit;
	bool humHighAlarm;
	bool humLowAlarm;
	unsigned long int humFirstOOR;
	unsigned long int humAlarmGracePeriod;
	
	void update(int sensor, int tempRawValue, int humRawValue, unsigned long int currentTimeElapsed);
	bool tempInRange();
	bool humInRange();
	void procAlarms(unsigned long int currentTimeElapsed);
	void procAlarms(int sensor, unsigned long int currentTimeElapsed);
	void clearAlarms();
	
  float getMaxTemp();
  float getMaxHum();
	float getTemp(int sensor);
  float getHum(int sensor);
	
	int maximum(int a, int b);
	
};

#endif