
  //// Use these if compiling from Arduino IDE ////
#include <EEPROM.h>
#include <Wire.h>
#include "printf.h"
#include "DeviceSettings.h"
#include "Satellite.h"
#include "BME280.h"
#include "mechElec.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

  //// Hysteresis values in deci-units
const uint8_t TEMPHYS = 1;  
const uint8_t HUMHYS = 5; 
const uint16_t SENSORPOLLPERIOD = 1000;
const uint16_t SATELLITELOOPPERIOD = 500;

  //// Global variable to store address of current sensor to poll
//uint8_t sensorAddress[2] = {0x77, 0x76};
uint8_t sensorAddress[2] = {0x76, 0x76};



Satellite standalone;

  //// OLED SCREEN
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void displayCommsError() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5,10);
  display.print("COMMS FAIL");
  display.display();
}

void updateDisplay(byte temp, byte hum) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  //display.setCursor(10,6);
  display.setCursor(8,6);
  
  display.print(temp);
  display.print(char(247));
  display.print(USEFREEDOMUNITS ? "F " : "C ");
  display.print(hum);
  display.println("%rh");
  display.setTextSize(1);
  display.print(" SP:");
  display.print(TEMPSP/10);
  display.print(".");
  display.print(TEMPSP%10);
  display.print(char(247));
  display.print(USEFREEDOMUNITS ? "F SP:" : "C SP:");
  display.print(HUMSP/10);
  display.print(".");
  display.print(HUMSP%10);
  display.print("%");
  
  display.display();
}

void setup(void) {

 //// Set buzzer, fan and heater pinModes
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(HEATPIN, OUTPUT);

  
    //// Start the serial service.
  Serial.begin(57600);

    ////Initialise Satellite object deviceIDs
  
    standalone.deviceID = 0;

     //// Start the OLED display service
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  
  //// BME280 and I2C Setup
  
  uint8_t osrs_t = 1;             //Temperature oversampling x 1
  uint8_t osrs_p = 1;             //Pressure oversampling x 1
  uint8_t osrs_h = 1;             //Humidity oversampling x 1
  uint8_t mode = 3;               //Normal mode
  uint8_t t_sb = 5;               //Tstandby 1000ms
  uint8_t filter = 0;             //Filter off 
  uint8_t spi3w_en = 0;           //3-wire SPI Disable

  uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
  uint8_t config_reg = (t_sb << 5) | (filter << 2) | spi3w_en;
  uint8_t ctrl_hum_reg = osrs_h;

  Wire.begin();

  writeReg(0xF2, ctrl_hum_reg, sensorAddress[0]);
  writeReg(0xF4, ctrl_meas_reg, sensorAddress[0]);
  writeReg(0xF5, config_reg, sensorAddress[0]);
  readTrim(sensorAddress[0]);

  printf_begin();
  printf("ROLE: %s with ID %i\n\r", standalone.deviceID);

  beep(); // Indicate successful startup (ie no serial lockup)
}


void loop(void) {

    int sensorCount = 1;
    long long unsigned int lastSensorPoll = 0;
    int temp_act[2] = {0.0, 0.0};
    int hum_act[2] = {0.0,0.0};
    signed long int temp_cal;
    unsigned long int hum_cal;
    
    for (int i = 0; i < sensorCount; i++) {
      readTrim(sensorAddress[i]);
      readData(sensorAddress[i]);

      temp_cal = calibration_T(temp_raw);
      hum_cal = calibration_H(hum_raw);
    }

   
    
      if ((millis() - lastSensorPoll > SENSORPOLLPERIOD) || millis() < 1000UL) { //If it's time to poll the sensors again
        lastSensorPoll = millis();
        
        // Read the temp and humidity, and send a Transmission packet whenever the change is sufficient.
      for (int i = 0; i < sensorCount; i++) {
        readTrim(sensorAddress[i]);
        readData(sensorAddress[i]);

        temp_cal = calibration_T(temp_raw);
        hum_cal = calibration_H(hum_raw);
        standalone.tempRawValue[0] = temp_cal * 10 / 100; //Convert raw values to actual.  use round(value*10)/10(.0?) to get 1dp
        standalone.humRawValue[0] =  hum_cal * 10 / 1024;

        if (USEFREEDOMUNITS) {
          standalone.tempRawValue[0] = (standalone.tempRawValue[0] * 9 / 5 + 320);
        }
      } 
  
        printf("Just read temp=%i, hum=%i\n and           %i,     %i\n", 
               standalone.tempRawValue[0], standalone.humRawValue[0], standalone.tempRawValue[1], standalone.humRawValue[1]);
        }

        updateDisplay(int(standalone.getMaxTemp()), int(standalone.getMaxHum())); // Update the OLED display
        
      
        
        // Drive fans 
        if (standalone.humRawValue[0] < HUMLOWLIMIT) {
          startFan();
          printf("Fan Start\n");
        }
        else if (standalone.humRawValue[0] >= HUMHIGHLIMIT) {
          stopFan();
          printf("Fan Stop\n");
        }

        //Drive heater
        if (standalone.tempRawValue[0] < TEMPLOWLIMIT) {
          startHeat();
          printf("Heat Start\n");
        }
        else if (standalone.tempRawValue[0] >= TEMPHIGHLIMIT) {
          stopHeat();
          printf("Heat Stop\n");
        }
        
        delay(SATELLITELOOPPERIOD); // Loop poll rate.  Adjust sensor poll rate later to match to reduce power consumption.
}
