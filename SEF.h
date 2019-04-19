/**
  \file SEF.h
  Library for the usage of SEFtool.
*/
#ifndef _SEF_
#define _SEF_

#define INDOOR 0
#define OUTDOOR 1

/**
  Initialization function of the library.
  Must be called in setup().
*/
void initSEF();

/**
  Verifies that the proximity sensor on the device is functioning.
  \returns true if the sensor is responding, false otherwise
*/
bool sensorActive();

/**
  Sends an audio warning to the user.
  Tone and duration of the warning can be controlled through the parameters.

  \param frequency Frequency of the audio warning in Hertz
  \param millis Duration of the warning in milliseconds
*/
void audioWarning(unsigned int frequency, unsigned int millis);

/**
  Generates a warning to the user according to the argument given.
  Depending on the argument, the function decides the behavior of the device.
  For a correct behavior of the device, the parameter given must be the return value of the last call to obstacleDetected().

  For debugging purposes: if the argument value is true, then the device will behave as if an obstacle has been detected.
  A value of false will have the opposite effect.

  \param detectionResult The return value of the last call to obstacleDetected().
*/
void userWarning(bool detectionResult);

/**
  Checks if the battery of the device is running low.

  \returns true if the battery is low, false otherwise
*/
bool batteryLow();

/**
  Reads the distance currently detected by the proximity sensor.

  \return The distance detected from the current object in centimeters
*/
unsigned int readDistance();

/**
  Tells if the there's an obstacle ahead of the user in a high level way.

  \return true if an obstacle has been detected, false otherwise
*/
bool obstacleDetected();

/**
  Sets the operating mode to either indoors or outdoors.
  The mode is by default initialized to outdoors.

  \param mode The mode in which the device operates. Can either be OUTDOOR or INDOOR.
*/
void setMode(int mode);

/**
  Returns the current threshold for the obstacle detection.
  Anything below such threshold is considered as obstacle. Entities above this distance are not considered.

  \return Current threshold for the obstacle detection
*/
int getThreshold();

#endif
