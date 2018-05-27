#include "AutomationProject.h"

AutomationProject::AutomationProject(float temp, int stepPin, int dir)
{
  _temp = temp;
  _step = stepPin;
  _dir = dir;
  _stepCounter = 0;
}

void AutomationProject::begin(int stepPin, int dir, int relay)
{
  //sets Digital Pins
  pinMode(stepPin, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(relay, OUTPUT);
}

float AutomationProject::readline(int readch, char *buffer, int len)
{
  //returns Bluetooth readings
  static int pos = 0;
  int rpos;

  if(readch > 0)
  {
    switch(readch)
    {
      case '\n':
      break;
      case '\r':
      rpos = pos;
      pos = 0;
      return rpos;
      default:
      if(pos < len-1)
      {
        buffer[pos++] = readch;
        buffer[pos] = 0;
      }
    }
  }
  return -1;
}
void AutomationProject::turnOnHeating()
{
  //sets heating
  digitalWrite(_dir, LOW);
  while(_stepCounter < 12);
     {
      digitalWrite(_step, HIGH);
      delayMicroseconds(10000);
      digitalWrite(_step, LOW);
      delayMicroseconds(10000);
      _stepCounter++;//stepCounter is used to keep track of motor's position
     }
}
void AutomationProject::turnOffHeating()
{
  //turns heating off
  digitalWrite(_dir, HIGH);
  while(_stepCounter > 0)
     {
      digitalWrite(_step, HIGH);
      delayMicroseconds(10000);
      digitalWrite(_step, LOW);
      delayMicroseconds(10000);
      _stepCounter--;//stepCounter is used to keep track of motor's position
     }
}
float AutomationProject::tempReading(float tempRead)
{
  //amends heating to temperature standard of room
  if(tempRead > _temp)//determines if room temperature is less or more than temperature standard
  {
    /*below compares room temperature to the temperature standard, by doing this the system allows for variences in 
    temperature changes which are dependant on temperature differences*/
    int pulse = tempRead - _temp;
    if(pulse > 6)
    {
      pulse = 6;//this is used to restrict temperature shifts
    }
    digitalWrite(_dir, HIGH);
    for(int i = 0; i < pulse; i++)
     {
      digitalWrite(_step, HIGH);
      delayMicroseconds(10000);
      digitalWrite(_step, LOW);
      delayMicroseconds(10000);
      _stepCounter--;//stepCounter is used to keep track of motor's position
     }
     return(tempRead);
  }
  else if(tempRead < _temp)
  {
    /*below compares room temperature to the temperature standard, by doing this the system allows for variences in 
    temperature changes which are dependant on temperature differences*/
   int pulse = _temp - tempRead;
   if(pulse > 6)
    {
      pulse = 6;//this is used to restrict temperature shifts
    }
   digitalWrite(_dir, LOW);
   for(int i = 0; i < pulse; i++)
     {
        digitalWrite(_step, HIGH);
        delayMicroseconds(10000);
        digitalWrite(_step, LOW);
        delayMicroseconds(10000);
        _stepCounter++;//stepCounter is used to keep track of motor's position
     }
     return(tempRead);
  }
  else 
  { 
    return(tempRead);
  }
}
void AutomationProject::ecoTempState(float temp)
{
  /*This is used to reduce temperature during inactivity, the temperature is not allow
    to fall below 4*C of the standard so that standard could be quickly reached again once activity returns*/
  if(_temp > (temp-4))
  {
      _temp = _temp -1;
      Serial.print("inactivity, new temperature = ");
      Serial.println(_temp);
  }
}
boolean AutomationProject::ecoTempState2(float temp)
{
  //This is used to reduce temperature during overcrowding, temperature is prevented from falling 2*C below standard
  if(_temp > (temp-2))
  {
      _temp = _temp -1;
      Serial.print("Room Crowded, new temperature = ");
      Serial.println(_temp);
  }
  return true;
}
boolean AutomationProject::resetTemp(float temp)
{
  //This function is used to reset temperatures
  if(temp > _temp)
  {
  _temp = temp;
  return true;
  }
  else
  {
    return false;
  }
}
void AutomationProject::printDigits(uint8_t digits){
  //This function is used for printing seconds and minutes, any time less than 10 is given a '0' in front of the number
  Serial.begin(9600);
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits, DEC);
}
boolean AutomationProject::noMotion(String startTime, String endTime, int relayPin)
{
      Serial.begin(9600);
      digitalWrite(relayPin, LOW);//Turns off lighting during no motion

      //Reports Activity
      Serial.println("Activity Report:");
      Serial.print("Room Entered - ");
      Serial.println(startTime);
      Serial.print("Activity Ended - ");
      Serial.println(endTime);
      Serial.println(" ");
      return false;
}
void AutomationProject::ReadingsPerHour(int currentTime)
{
    Serial.begin(9600);

    //Here incrementor data is used and converted into time data
    int hours = ((currentTime * 5) / 60) / 60;
    int minutes = ((currentTime * 5) / 60) - hours * 60;
    int secs = currentTime * 5 - minutes * 60;

    //Reports Activity
    Serial.println("Activity Report:");
    Serial.print("Time in Office Today: ");
    Serial.print(hours);
    printDigits(minutes);
    printDigits(secs);
    Serial.println(" ");
}
boolean AutomationProject::DoorReadings(String outputTime, boolean activityPresent)
{
  //Reports activity on door
  Serial.print(outputTime);
  Serial.println(" - Room Entered");
   if (activityPresent == false)
  {
    return true;
  }
  else
  {
    return false;
  }
}

