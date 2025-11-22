#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"
#include "status_led.h"
#include "scale.h"

/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  while(1) {
    status_led_update(get_hydration_state());  // Update LED based on current hydration state
    vTaskDelay(pdMS_TO_TICKS(2000));  // Update every 2 seconds
  }
}

/*
Speaker that audibly alerts the user when the LED is red and the user needs to drink water
*/
void taskAlertUser(void *pv) {
  while(1) {
    // Check if hydration level requires alert
    if (get_hydration_state() == CRITICAL) {
      speaker_alert();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));  // Check every second
  }
}

/*
Generates HTML page allowing user to view hydration data and interact with system wirelessly through Bluetooth
*/
void taskHTMLPage(void *pv) {
  while(1) {
    // HTML server logic - TO BE IMPLEMENTED
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Detects scale changes and stores meaningful changes in non-volatile memory
*/
void taskReadScale(void *pv) {
  float prev_weight = 0;
  while (1) {
    float current_weight = scale_read_weight();
    //if there is something on the scale, the scale weight decreased, and the difference in weight is meaningful
    if (current_weight > 30.0 && current_weight < prev_weight && prev_weight - current_weight > 30.0)
    {
      record_grams_drank(prev_weight - current_weight);
    }
    if (current_weight > 30.0)
      prev_weight = current_weight;
    update_hydration_status();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Turns sleep mode on if no meaningful weight changes occur for x seconds, and wakes up when there is
*/
void taskPowerManager(void *pv) {
  while(1) {
    // Power management logic - TO BE IMPLEMENTED
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup() {
  Serial.begin(115200);
  scale_init();
  status_led_init();
  speaker_init();
  // xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 1, NULL);
  // xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 1, NULL);
  xTaskCreate(taskReadScale, "taskReadScale", 2048, NULL, 1, NULL);
  xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 2, NULL);
  xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 3, NULL);
}

void loop() {

}