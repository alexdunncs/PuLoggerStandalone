#ifndef MECHELEC
#define MECHELEC

#include <Arduino.h>

const int BUZZERPIN = 2;
const int FANPIN = 5;
const int HEATPIN = 4;

void setBuzzer(bool state);
void beep();

void startFan();
void stopFan();

void startHeat();
void stopHeat();

#endif