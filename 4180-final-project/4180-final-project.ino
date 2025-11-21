#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"

bool alert_enabled = true;  // Global flag to enable/disable alerts

/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  while(1) {
    HydrationState state = hydration_get_state();
    
    // Your LED logic here based on state
    // HYDRATION_GOOD = green
    // HYDRATION_OK = yellow
    // HYDRATION_LOW = orange
    // HYDRATION_CRITICAL = red
    
    vTaskDelay(pdMS_TO_TICKS(1000));
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
        Serial.println("⚠️ ALERT: Please drink water!");
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
    // HTML server logic
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Detects scale changes and stores meaningful changes in non-volatile memory
*/
void taskReadScale(void *pv) {
  while(1) {
    // Scale reading logic
    // Example: float weight_change = scale_read_grams();
    // hydration_update_from_weight(weight_change);
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Turns sleep mode on if no meaningful weight changes occur for x seconds, and wakes up when there is
*/
void taskPowerManager(void *pv) {
  while(1) {
    // Power management logic
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

/*
Test task to simulate drinking water
*/
void taskTestHydration(void *pv) {
  vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 seconds after boot
  
  while(1) {
    Serial.println("\n--- Test Menu ---");
    Serial.println("Type in Serial Monitor:");
    Serial.println("'d' - Simulate drinking 200ml");
    Serial.println("'r' - Reset hydration for the day");
    Serial.println("'s' - Show current status");
    Serial.println("'a' - Toggle alerts on/off");
    
    vTaskDelay(pdMS_TO_TICKS(10000));  // Show menu every 10 seconds
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== Hydration Tracker Starting ===");
  
  // Initialize modules
  hydration_init();
  speaker_init();
  
  // Create FreeRTOS tasks
  xTaskCreate(taskUpdateStatusLED, "StatusLED", 2048, NULL, 1, NULL);
  xTaskCreate(taskAlertUser, "AlertUser", 2048, NULL, 2, NULL);
  xTaskCreate(taskTestHydration, "TestHydration", 2048, NULL, 1, NULL);
  // xTaskCreate(taskReadScale, "ReadScale", 4096, NULL, 2, NULL);
  // xTaskCreate(taskHTMLPage, "HTMLPage", 8192, NULL, 1, NULL);
  // xTaskCreate(taskPowerManager, "PowerManager", 2048, NULL, 1, NULL);
  
  Serial.println("All tasks created");
  Serial.println("\n🚰 Hydration tracking active!");
  Serial.println("⚠️ Starting in CRITICAL state - alerts will sound every 30 seconds");
  Serial.println("\nOpen Serial Monitor and type commands to test!\n");
}

void loop() {
  // Handle serial commands for testing
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    switch(cmd) {
      case 'd':
      case 'D':
        Serial.println("\n💧 Simulating drinking 200ml...");
        hydration_add_intake(200);
        Serial.printf("Progress: %d%%\n", hydration_get_percentage());
        break;
        
      case 'r':
      case 'R':
        Serial.println("\n🔄 Resetting daily hydration...");
        hydration_reset_daily();
        break;
        
      case 's':
      case 'S':
        Serial.println("\n📊 Current Status:");
        Serial.printf("Intake: %.0f ml / %.0f ml\n", 
                     hydration_get_current_ml(), 
                     hydration_get_goal_ml());
        Serial.printf("Progress: %d%%\n", hydration_get_percentage());
        Serial.printf("State: ");
        switch(hydration_get_state()) {
          case HYDRATION_GOOD: Serial.println("✅ GOOD"); break;
          case HYDRATION_OK: Serial.println("👍 OK"); break;
          case HYDRATION_LOW: Serial.println("⚠️ LOW"); break;
          case HYDRATION_CRITICAL: Serial.println("🚨 CRITICAL"); break;
        }
        Serial.printf("Alerts: %s\n", alert_enabled ? "ENABLED" : "DISABLED");
        break;
        
      case 'a':
      case 'A':
        alert_enabled = !alert_enabled;
        Serial.printf("\n🔔 Alerts %s\n", alert_enabled ? "ENABLED" : "DISABLED");
        break;
    }
  }
  
  delay(100);
}