#include "mechElec.h"

void setBuzzer(bool state) {
  digitalWrite(BUZZERPIN,(state ? HIGH : LOW));
}

void beep() {
  setBuzzer(true);
  delay(20);
  setBuzzer(false);
}

void startFan() {
	digitalWrite(FANPIN, 1);
}

void stopFan() {
	digitalWrite(FANPIN, 0);
}

void startHeat() {
	digitalWrite(HEATPIN, 1);
}

void stopHeat() {
	digitalWrite(HEATPIN, 0);
}

