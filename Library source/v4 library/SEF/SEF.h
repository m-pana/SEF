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
  Reads the distance currently detected by the proximity sensor.
  The reading is only performed once, without further security checks.
  \return The distance detected from the current object in centimeters
*/
unsigned int readDistance();

/**
  Tells if the there's an obstacle ahead of the user in a high level way.
  \return true if an obstacle has been detected, false otherwise
*/
bool obstacleDetected();

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
  Generates a warning to the user according to the argument given.
  Depending on the argument, the function decides the behavior of the device.
  For a correct behavior of the device, the parameter given must be the return value of the last call to obstacleDistance().
  Depening on the distance of the obstacle, the feedback given to the user will be different.
  \param detectionResult The return value of the last call to obstacleDistance().
*/
void userWarningVariable(int detectionResult);

/**
  Checks if the battery of the device is running low.
  \returns true if the battery is low, false otherwise
*/
bool testBatteryLow();

/**
  Verifies that the proximity sensor on the device is functioning.
  \returns true if the sensor is responding, false otherwise
*/
bool testSensorActive();

/**
  Verifies that the vibrating unit on the device is functioning.
  \returns true if the vibrating unit is connected, false otherwise
*/
bool testVibrationActive();

/**
  Returns the current threshold for the obstacle detection.
  Anything below such threshold is considered as obstacle. Entities above this distance are not considered.
  \return Current threshold for the obstacle detection
*/
int getThreshold();

/**
  Reads and returns the current distance from an obstacle.
  Multiple readings are performed to ensure that the reading is correct.
  If there are more positives than negatives, the positives are averaged and the result is returned.
  If there are more negatives than positives, the negatives are averaged and the result is returned.
  \return the best estimation possible for the distance of an obstacle ahead.
*/
int obstacleDistance(int countBatteryWarnings);

/**
  Sends an audio warning to the user and PERMANENTLY STOPS THE DEVICE.
*/
void malfunctionHalt();

/**
 * Interrupt called function
 * Changes the mode of the device between indoor and outdoor
 */
void toggleMode();

/**
  Sets the operating mode to either indoors or outdoors.
  The mode is by default initialized to outdoors.
  \param mode The mode in which the device operates. Can either be OUTDOOR or INDOOR.
*/
void setMode(int mode);  

void beeping(int frequency);




#endif