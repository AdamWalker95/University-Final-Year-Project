/*This program is implemented to operate as a receiver for sensor data
and make changes within the environment sensors are located*/

/*Set of libraries used for this program, some of which 
are externally downloaded, and one of which that was 
built specifically for this program (AutomationProject.h)*/
#include "AutomationProject.h"
#include <TimedAction.h>
#include<SoftwareSerial.h>
#include <Wire.h>
#include "Sodaq_DS3231.h";

const float temperature = 18.00; //This sets temperature standard for room
const int stepPin = 4; //Digital Pin 4 is to connect to the step pin of the stepper driver 
const int dirPin = 5; //Digital Pin 5 controls the direction of the stepper motor
const int relayPin = 2; //Digital Pin 2 is the output for the relay controlling the lighting
float currentTemperature; //This is the temperature readings the system recieves
    static char buffer[80];

//Below are a set of triggers used, most of which are required for a function called 'TimedAction'
boolean inactivityTrigger, changeMotion, addIncrement, activityPresent, ecoState2, hourReading = false; 

//Below are a set of variables to hold time related readings
String startTime, endTime, startLongest, endLongest = " "; 
int currentTime, temporaryTime, longestTime = 0;

/*Below are a set of functions which couldn't be placed within the externally built library
this was mostly due to the operation of the TimedAction function which required their use*/
void inactivityFunction(), motionOutput(), noMotionOutput(), getIncrement(), onHourReadings();
void tempReset(float currentTemperature);

SoftwareSerial BTSerial(10, 11); //This sets up the Bluetooth readings
AutomationProject obj(temperature, stepPin, dirPin); //declares object of the system library
TimedAction inactivityTimer = TimedAction(1800000, inactivityFunction);//Timer is used to reduce temperature during periods of inactivity
TimedAction motionTimer = TimedAction(60000, noMotionOutput);//Timer is set to check for motion
TimedAction increment = TimedAction(5000, getIncrement);//incrementor used for time-based functions
TimedAction hourlyReadings = TimedAction(3200000, onHourReadings);//Feedback Display set to report on an hourly basis

void setup() {
  Serial.begin(9600);//sets baud rate for Serial Outputs
  obj.begin(stepPin, dirPin, relayPin); //function sets up pins
  BTSerial.begin(38400);//sets baud rate for Bluetooth
  Wire.begin();
  rtc.begin();//rtc is a function used for time reading
  obj.turnOnHeating(); //function sets heating
  digitalWrite(relayPin, LOW); //sets lighting to off
}

void loop() {
  DateTime now = rtc.now();//gets time

  //system does not operate outside office hours
 // if ((now.hour() > 8 || (now.hour() == 8 && now.minute() >= 30)) && (now.hour() < 17 || (now.hour() == 17 && now.minute() < 30)))
  {
    //checks timers
    inactivityTimer.check();
    motionTimer.check();
    hourlyReadings.check();
    
    if (obj.readline(BTSerial.read(), buffer, 80) > 0)//function retrieves Bluetooth readings
    {
      String str (buffer);
      if (str.equals("motion"))
      {
        //if motion is registered all energy saving settings are reset and activity is reported present
        inactivityTimer.reset();
        tempReset();
        motionTimer.reset();
        activityPresent = true;
      }
      else if (str.equals("reed"))
      {
        reedOutput();
        digitalWrite(relayPin, HIGH);//turns on lighting when door sensor is triggered
      }
      else
      {
        //Below is used to retrieve temperature readings
        float tempRead = atof(buffer);
        currentTemperature = obj.tempReading(tempRead);
        Serial.print("Current Temperature = ");
        Serial.println(currentTemperature);
      }
    }
    if (activityPresent == true)
    {
      increment.check(); //increments during activity
    }
  }
  /*else
  {
    obj.turnOffHeating();//outside of office hours heating is switched off
  }*/
}
void inactivityFunction()
{
  inactivityTrigger ? inactivityTrigger = true : inactivityTrigger = false;
  {
    obj.ecoTempState(temperature);//if inactivity has progressed for long enough room temperature is reduced
  }
}
void tempReset()
{
  if (obj.resetTemp(temperature)) //checks if temperature of room has been lowered below standard
  {
    obj.tempReading(currentTemperature);//resets temperature (if required)
  }
}

void noMotionOutput()
{
  changeMotion ? changeMotion = true : changeMotion = false;
  {
    if (activityPresent == true)
    {

      DateTime now = rtc.now();
      sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
      String endTime(buffer);//records time of activity ending
      activityPresent = obj.noMotion(startTime, endTime, relayPin); //function turns off lighting and gives activity report
      if (temporaryTime > longestTime)//records longest period within office
      {
        startLongest = startTime;
        endLongest = endTime;
        longestTime = temporaryTime;
      }
      temporaryTime = 0;


      if (ecoState2 == true)
      {
        tempReset();//resets temperature (if required)
      }
    }
  }
}
void reedOutput()
{
  DateTime now = rtc.now();
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  String outputTime(buffer);
  obj.DoorReadings(outputTime);
  if (activityPresent == false)
  {
    startTime = outputTime;//records time when activity begins
    activityPresent = true;
  }
  else
  {
  ecoState2 = obj.ecoTempState2(temperature);//reduces temperature during overcrowding
  }
}
void getIncrement()
{
  //incrementor for time-based functions
  addIncrement ? addIncrement = true : addIncrement = false;
  {
    currentTime++;
    temporaryTime++;
  }
}
void onHourReadings()
{
  hourReading ? hourReading = true : hourReading = false;
  {
    obj.ReadingsPerHour(currentTime);//activity report used hourly
  }
}

