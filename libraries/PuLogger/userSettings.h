#ifndef USERSETTINGS
#define USERSETTINGS

const bool USEFREEDOMUNITS = true;

	//// Alarm thresholds are defined here
const int TEMPSP = 780;
const int HUMSP = 700;
const int TEMPHIGHLIMIT = TEMPSP + 5*2; //*2 factors are for farenheit
const int TEMPLOWLIMIT = TEMPSP - 2*2;
const int HUMHIGHLIMIT = HUMSP + 10;
const int HUMLOWLIMIT = HUMSP - 5;
const unsigned long int TEMPALARMGRACEPERIOD = (1000UL *  60UL * 60UL * 5); //5 hours (4.5hrs recovery measured from 24C, which should be far below worst-case in normal use)
const unsigned long int HUMALARMGRACEPERIOD = (1000UL * 60UL * 60UL * 2); //2 hours (1hr recovery measured, with 100% tolerance)



#endif