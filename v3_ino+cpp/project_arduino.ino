#include <SEF.h>

bool detectionResult = false;

void setup() {
  initSEF();
  Serial.begin(9600);
}

void loop() {
  
  bool sensorWorks = sensorActive();
  bool vibrationWorks = vibrationActive();  

  if( (sensorWorks == true) && (vibrationWorks == true) ) 
  {
      detectionResult = obstacleDetected();
    
      // printing to serial monitor for debugging purposes
      Serial.flush();
      String out = (detectionResult ? "TRUE" : "FALSE");
      Serial.println(out);
      Serial.flush();
      
      userWarning(detectionResult); 
  }
  else 
  {
      LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
  }
 


}
