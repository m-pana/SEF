#include <SEF.h>
#include <LowPower.h>

int detectionResult = 0;
char out[100];
int countBatteryWarnings = 0; 


void setup() {
  initSEF();
  Serial.begin(9600);


}

void loop() {
  
  bool sensorWorks = sensorActive();
  bool vibrationWorks = true; //vibratorActive();  

  if( (sensorWorks == true) && (vibrationWorks == true) ) 
  {
    detectionResult = obstacleDistance(countBatteryWarnings);
    
    // printing to serial monitor for debugging purposes
    Serial.flush();
    sprintf(out, "%d, Threshold: %d, Sub-limits: %d, %d", detectionResult, getThreshold(), getThreshold()/3, 2*getThreshold()/3);
    Serial.println(out);
    Serial.flush();
    
    if ( testBatteryLow() == true ) 
    {
      countBatteryWarnings = countBatteryWarnings + 1; 
    }
    
  
    userWarningVariable(detectionResult);
      /* --- BOOLEAN VERSION OF DETECTION (IMPLEMENTAION A)
       * detectionResult = obstacleDetected();
    
      // printing to serial monitor for debugging purposes
      Serial.flush();
      String out = (detectionResult ? "TRUE" : "FALSE");
      Serial.println(out);
      Serial.flush();
      
      userWarning(detectionResult); */
  }
  else 
  {   
      //beep to avert user of the malfunctioning 
       beeping(5000);       
      LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
  }
 


}
