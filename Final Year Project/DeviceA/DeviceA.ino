/*This program is implemented to operate  sensors and send
  data received from sensors to an external program for
  regulating environmental changes*/

/*Set of libraries used for this program, one of which that was externally downloaded.
  A specifically built library for use with this program was considered, however its
  use became too tedious and had little effect on reducing the size of the main program*/
#include <SoftwareSerial.h>
#include <TimedAction.h>

const int motionPin = 5;//Digital Pin 5 is set to receive motion readings
const int reedPin = 7;//Digital Pin 7 is set to receive readings from a reed switch connected to a door, recording activity of door
const int tempPin = 1;//Analog Pin 1 is set to receive readings from a temperature reader

//Below variables are integrated for use with a function called 'TimedAction'
boolean checkTemperature, motionReady, reedReady = true;
boolean motionDisabled, reedDisabled = false;

SoftwareSerial BTSerial(10, 11);//Sets up Bluetooth
void temperatureReader(), motionTimed(), reedTimed();//Functions that are to be used with TimedAction

TimedAction temperatureTimer = TimedAction(30000, temperatureReader);//This timer is used to take temperature readings
TimedAction motionTimer = TimedAction(5000, motionTimed);//This timer is used to reduce redundancy in motion readings
TimedAction reedTimer = TimedAction(5000, reedTimed);//This timer is used to reduce redundancy in reed switch readings
void setup() {
  pinMode(motionPin, INPUT);//This sets up motion reader
  pinMode(reedPin, INPUT);//This sets up reed switch
  BTSerial.begin(38400);//This sets Bluetooth baud rate to 38400
}

void loop() {
  temperatureTimer.check();//This checks temperature readings

  /*This allows for one reading on door activity per timeframe, this is necessary for use within the reciever program
    as if the program was to repeatedly send readings on door activity it will affect an energy reduction function used
    within the reciever program during periods of overcrowding within the office*/
  if(reedDisabled == false)
  {
    doorCheck();//checks door readings
  }
  else
  {
    reedTimer.check();//function causes system to wait until door readings can be checked again
  }

  /*This allows for one reading on motion per timeframe, this helps reduce redundancy of 
    unnecessary data readings*/
  if(motionDisabled == false)
  {
    motion();//checks motion readings
  }
  else
  {
    motionTimer.check();//function causes system to wait until motion readings can be checked again
  }
}
void temperatureReader()
{
  //Converts temperature readings into readable data and sends data via Bluetooth
  checkTemperature ? checkTemperature = true : checkTemperature = false;
  {
  int value = analogRead(tempPin);
  float millivolts = value * (3300/1024);
  float temperature = (millivolts - 500)/100;
  BTSerial.println(temperature);
  }
}
void motionTimed()
{
  motionReady ? motionReady = true : motionReady = false;
  {
  motionDisabled = false;
  }
}
void reedTimed()
{
  reedReady ? reedReady = true : reedReady = false;
  {
  reedDisabled = false;
  }
}
void motion()
{
  //Sends motion readings via Bluetooth during activity
  if(digitalRead(motionPin) == HIGH)
  {
    BTSerial.println("motion");
    motionDisabled = true;
  }
}
void doorCheck()
{
  //Sends reed switch readings via Bluetooth during activity
  if(digitalRead(reedPin) == HIGH)
  {
    BTSerial.println("reed");
    reedDisabled = true;
  }
}
