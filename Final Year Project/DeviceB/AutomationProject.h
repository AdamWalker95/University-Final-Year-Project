#include "Arduino.h"

class AutomationProject
{
  public:
  AutomationProject(float temp, int stepPin, int dir);
  void begin(int stepPin, int dir, int relay);
  void turnOnHeating();
  void turnOffHeating();
  void printDigits(uint8_t digits);
  float readline(int readch, char *buffer, int len);
  float tempReading(float tempRead);
  void ecoTempState(float temp);
  boolean ecoTempState2(float temp);
  boolean resetTemp(float temp);
  boolean noMotion(String startTime, String endTime, int relayPin);
  void ReadingsPerHour(int currentTime);
  void AutomationProject::DoorReadings(String outputTime);

  private:
  float _temp;//This variable holds set temperature of room
  int _step;
  int _dir;
  int _stepCounter;//This variable keeps track on motor position
};

