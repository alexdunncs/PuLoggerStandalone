#include "Satellite.h"

Satellite::Satellite() : deviceID(-1), hasAdditionalSensor(false), deviceUp(false), lastTransmission(0UL), 
	tempRawValue{-1,-1}, humRawValue{-1,-1}, tempRawAvg(-1), humRawAvg(-1),
	tempHighLimit(TEMPHIGHLIMIT), tempLowLimit(TEMPLOWLIMIT), tempHighAlarm(false), tempLowAlarm(false), 
	tempFirstOOR(0UL), tempAlarmGracePeriod(TEMPALARMGRACEPERIOD),
	humHighLimit(HUMHIGHLIMIT), humLowLimit(HUMLOWLIMIT), humHighAlarm(false), humLowAlarm(false), 
	humFirstOOR(0UL), humAlarmGracePeriod(HUMALARMGRACEPERIOD) 
{
	
}

Satellite::Satellite(int deviceID, bool hasAdditionalSensor, unsigned long int tGrace, int tHigh, int tLow, unsigned long int hGrace, int hHigh, int hLow) : 
	deviceID(deviceID), hasAdditionalSensor(hasAdditionalSensor), deviceUp(false), lastTransmission(0UL), 
	tempRawValue{-1,-1}, humRawValue{-1,-1}, tempRawAvg(-1), humRawAvg(-1),
	tempHighLimit(TEMPHIGHLIMIT), tempLowLimit(TEMPLOWLIMIT), tempHighAlarm(false), tempLowAlarm(false), 
	tempFirstOOR(0UL), tempAlarmGracePeriod(TEMPALARMGRACEPERIOD),
	humHighLimit(HUMHIGHLIMIT), humLowLimit(HUMLOWLIMIT), humHighAlarm(false), humLowAlarm(false), 
	humFirstOOR(0UL), humAlarmGracePeriod(HUMALARMGRACEPERIOD) 
{
	
}

	//// Update satellite snapshot from relevant fields of a transmission.
void Satellite::update(int sensor, int tempRawValue, int humRawValue, unsigned long int currentTimeElapsed) {
	Satellite::tempRawValue[sensor] = tempRawValue;
	Satellite::humRawValue[sensor] = humRawValue;
	Satellite::tempRawAvg = (Satellite::tempRawValue[0] + Satellite::tempRawValue[1]) / 2; // Updates averages
	Satellite::humRawAvg = (Satellite::humRawValue[0] + Satellite::humRawValue[1]) / 2;
	Satellite::lastTransmission = currentTimeElapsed;
	
	Satellite::procAlarms(currentTimeElapsed);
}
	//// Is temperature between the high and low limits?
bool Satellite::tempInRange() {
	bool inRange = (tempRawValue[0] >= tempLowLimit && tempRawValue[0] <= tempHighLimit);
	if (hasAdditionalSensor && inRange) {
		inRange = (tempRawValue[1] >= tempLowLimit && tempRawValue[1] <= tempHighLimit);
	}
	return inRange;
}

	//// Is humidity between the high and low limits?
bool Satellite::humInRange() {
	bool inRange = (humRawValue[0] >= humLowLimit && humRawValue[0] <= humHighLimit);
	if (hasAdditionalSensor && inRange) {
		inRange = (humRawValue[1] >= humLowLimit && humRawValue[1] <= humHighLimit);
	}
	return inRange;
}

  //// Check alarm triggers and update alarm status
void Satellite::procAlarms(unsigned long int currentTimeElapsed) {
	procAlarms(0, currentTimeElapsed);
}
	
void Satellite::procAlarms(int sensor, unsigned long int currentTimeElapsed) {
	
	if (tempRawValue[sensor] < tempLowLimit) { // If value is out of bounds
		if (!tempFirstOOR) { // If it only just went OOB
			tempFirstOOR = currentTimeElapsed; // Record grace-period start-time.
		}
		else if ((currentTimeElapsed - tempFirstOOR) > tempAlarmGracePeriod) { // Otherwise, check if OOB state duration exceeds grace period.
			tempLowAlarm = true; //If so, set alarm state.
		}
		else {} // If not, wait out the grace period
	}
	else if (tempInRange()){ // If the value is within bounds
		tempFirstOOR = 0UL; // Reset grace period start time to inactive state
	}
	
	if (tempRawValue[sensor] > tempHighLimit) { // As above
		if (!tempFirstOOR) {
			tempFirstOOR = currentTimeElapsed;
		}
		else if ((currentTimeElapsed - tempFirstOOR) > tempAlarmGracePeriod || true ) { //temp should never be too high
			tempHighAlarm = true;
		}
		else {} 
	}
	else if (tempInRange()){
		tempFirstOOR = 0UL;
	}
	
	if (humRawValue[sensor] < humLowLimit) { // As above
		if (humFirstOOR == 0UL) {
			humFirstOOR = currentTimeElapsed;
		}
		else if ((currentTimeElapsed - humFirstOOR) > humAlarmGracePeriod) {
			humLowAlarm = true;
		}
		else {} 
	}
	else if (humInRange()){
		humFirstOOR = 0UL;
	}
	
	if (humRawValue[sensor] > humHighLimit) { // As above
		if (!humFirstOOR) {
			humFirstOOR = currentTimeElapsed;
		}
		else if ((currentTimeElapsed - humFirstOOR) > humAlarmGracePeriod || true ) { //humidity should never be too high)
			humHighAlarm = true;
		}
		else {}
	}
	else if (humInRange()){
		humFirstOOR = 0UL;
	}
	
	if (hasAdditionalSensor) {
		procAlarms(1, currentTimeElapsed); // Recursive call for second sensor
	}
}

	////Reset alarm state and grace period tracking
void Satellite::clearAlarms() {
	tempLowAlarm = false;
	tempHighAlarm = false;
	//tempFirstOOR = 0;
	
	humLowAlarm = false;
	humHighAlarm = false;
	//humFirstOOR = 0;
}

float Satellite::getMaxTemp() {
	return  float(maximum(tempRawValue[0], tempRawValue[1])) / 10.0;
}

float Satellite::getMaxHum() {
	return  float(maximum(humRawValue[0], humRawValue[1])) / 10.0;
}

float Satellite::getTemp(int sensor) {
	return  float(tempRawValue[sensor]) / 10.0;
}

float Satellite::getHum(int sensor) {
	return  float(humRawValue[sensor]) / 10.0;
}

int Satellite::maximum(int a, int b) {
	return (a > b ? a : b);
}