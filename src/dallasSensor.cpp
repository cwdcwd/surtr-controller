/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/cwd/Dropbox/code/particle.io/dallasSensor/dallasSensor/src/dallasSensor.ino"
#include <DS18B20.h>
#include <math.h>

void setup();
void loop();
void publishData();
int relayOn(String val);
int relayOff(String val);
int setMin(String val);
int setMax(String val);
void getTemp();
#line 4 "/Users/cwd/Dropbox/code/particle.io/dallasSensor/dallasSensor/src/dallasSensor.ino"
const int      RELAY_PIN         = 16;
const int      MAXRETRY          = 4;
const uint32_t msSAMPLE_INTERVAL = 2500;
const uint32_t msMETRIC_PUBLISH  = 30000;

DS18B20  ds18b20(D2, true); //Sets Pin D2 for Water Temp Sensor and 
                            // this is the only sensor on bus
int   relayState=0;
char     szInfo[64];
double   celsius;
double   fahrenheit;
double  tempMax=79;
double  tempMin=60;
uint32_t msLastMetric;
uint32_t msLastSample;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  Particle.function("setMin", setMin);
  Particle.function("setMax", setMax);
  Particle.function("relayOff", relayOff);
  Particle.function("relayOn", relayOn);

  Particle.variable("relayState", relayState);
  Particle.variable("tempMax", tempMax);
  Particle.variable("tempMin", tempMin);
  Particle.variable("temp", fahrenheit);

  relayOff(" ");
}

void loop() {
  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    getTemp();
  }

  if (millis() - msLastMetric >= msMETRIC_PUBLISH){
    Serial.println("Publishing now.");

    if(fahrenheit>=tempMax) {
      relayOff(" ");
    } else if(fahrenheit<=tempMin) {
      relayOn(" ");
    }

    publishData();
  }
}

void publishData(){
  sprintf(szInfo, "%2.2f", fahrenheit);
  Particle.publish("currentTemp", szInfo, PRIVATE);
  String relayStateText = (relayState==0)?"OFF":"ON";
  Particle.publish("relayState",  relayStateText, PRIVATE);
  msLastMetric = millis();
}

int relayOn(String val) {
  relayState = 1; 
  digitalWrite(RELAY_PIN, HIGH);
  return 1;
}

int relayOff(String val) {
  relayState = 0; 
  digitalWrite(RELAY_PIN, LOW);
  return 1;
}

int setMin(String val)
{
  tempMin = val.toFloat();
  return 1;
}

int setMax(String val)
{
  tempMax = val.toFloat();
  return 1;
}

void getTemp(){
  float _temp;
  int   i = 0;

  do {
    _temp = ds18b20.getTemperature();
  } while (!ds18b20.crcCheck() && MAXRETRY > i++);

  if (i < MAXRETRY) {
    celsius = _temp;
    fahrenheit = ds18b20.convertToFahrenheit(_temp);
    Serial.println(fahrenheit);
  }
  else {
    celsius = fahrenheit = NAN;
    Serial.println("Invalid reading");
  }
  msLastSample = millis();
}
