#include <Arduino.h>
#include "scale.h"
#include "hydration.h"
/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  
}

/*
Speaker that audibly alerts the user when the LED is red and the user needs to drink water
*/
void taskAlertUser(void *pv) {
  
}

/*
Generates HTML page allowing user to view hydration data and interact with system wirelessly through Bluetooth
*/
void taskHTMLPage(void *pv) {

}

/*
Detects scale changes and stores meaningful changes in non-volatile memory
*/
void taskReadScale(void *pv) {
  while (1) {
    update_hydration_status(-1 * scale_read_delta());
    Serial.println(get_hydration_state());
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Turns sleep mode on if no meaningful weight changes occur for x seconds, and wakes up when there is
*/
void taskPowerManager(void *py) {

}

void setup() {
  Serial.begin(115200);
  scale_init();
  // xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 1, NULL);
  // xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 1, NULL);
  xTaskCreate(taskReadScale, "taskReadScale", 2048, NULL, 1, NULL);
}

void loop() {

}