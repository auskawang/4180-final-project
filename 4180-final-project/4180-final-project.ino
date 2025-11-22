#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"
#include "status_led.h"

bool alert_enabled = true;  // Global flag to enable/disable alerts

/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  while(1) {
    status_led_update();  // Update LED based on current hydration state
    vTaskDelay(pdMS_TO_TICKS(2000));  // Update every 2 seconds
  }
}

/*
Speaker that audibly alerts the user when the LED is red and the user needs to drink water
*/
void taskAlertUser(void *pv) {
  while(1) {
    // Check if hydration level requires alert
    if (alert_enabled && hydration_needs_alert()) {
      if (speaker_should_alert()) {
        speaker_alert();
        Serial.println(" ALERT: Please drink water!");
      }
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
  while (1) {
    update_hydration_status(-1 * scale_read_delta());
    Serial.println(get_hydration_state());
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

/*
Test task to simulate drinking water and test functionality
*/
void taskTestHydration(void *pv) {
  vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 seconds after boot
  
  while(1) {
    Serial.println("\n========== Test Menu ==========");
    Serial.println("Commands (type in Serial Monitor):");
    Serial.println("  'd' - Simulate drinking 200ml");
    Serial.println("  'D' - Simulate drinking 500ml");
    Serial.println("  'r' - Reset hydration for the day");
    Serial.println("  's' - Show current status");
    Serial.println("  'a' - Toggle alerts on/off");
    Serial.println("  't' - Test LED color sequence");
    Serial.println("  'l' - Toggle LED on/off");
    Serial.println("  'h' - Show this help menu");
    Serial.println("===============================\n");
    
    vTaskDelay(pdMS_TO_TICKS(15000));  // Show menu every 15 seconds
  }
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