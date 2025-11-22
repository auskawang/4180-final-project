#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"
#include "status_led.h"
#include "scale.h"
#include "storage.h"

/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  while(1) {
    status_led_update(get_hydration_state());
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

/*
Speaker that audibly alerts the user when the LED is red and the user needs to drink water
*/
void taskAlertUser(void *pv) {
  while(1) {
    if (get_hydration_state() == CRITICAL) {
      speaker_alert();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/*
Generates HTML page allowing user to view hydration data and interact with system wirelessly through Bluetooth
This is where you'll access the storage functions for your web interface
*/
void taskHTMLPage(void *pv) {
  while(1) {
    // HTML server logic - TO BE IMPLEMENTED
    
    // EXAMPLE: How to use storage in your web server
    // When user requests history page:
    /*
    DailyHydrationData history[STORAGE_DAYS];
    if (storage_get_history(history)) {
      // Send history array to HTML page
      // Convert to JSON or CSV for export
      for (int i = 0; i < STORAGE_DAYS; i++) {
        if (history[i].valid) {
          // Send: day, consumed, goal, percentage
        }
      }
    }
    */
    
    // When user clicks "Reset All Data" button:
    // storage_reset_all();
    
    // When user clicks "Save Today" button:
    // hydration_save_day();
    
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
    
    // If there is something on the scale, weight decreased, and difference is meaningful
    if (current_weight > 30.0 && current_weight < prev_weight && prev_weight - current_weight > 30.0) {
      record_grams_drank(prev_weight - current_weight);  // This updates today's consumption
    }
    
    if (current_weight > 30.0)
      prev_weight = current_weight;
      
    update_hydration_status();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Optional: Automatically save at end of each day
This task saves today's data at midnight or after a certain time period
*/
void taskDailySave(void *pv) {
  const uint32_t SAVE_INTERVAL = 24 * 60 * 60 * 1000;  // 24 hours in milliseconds
  
  while(1) {
    // Wait for end of day (24 hours)
    vTaskDelay(pdMS_TO_TICKS(SAVE_INTERVAL));
    
    // Save today's data to the 7-day history
    hydration_save_day();
    
    // Reset for new day
    hydration_reset();
    
    Serial.println("New day started - previous day saved to history");
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
  
  // ============ INITIALIZATION (Single calls) ============
  storage_init();        // Initialize NVS storage
  scale_init();          // Initialize HX711 scale
  status_led_init();     // Initialize RGB LED
  speaker_init();        // Initialize speaker
  hydration_init();      // Initialize hydration tracking
  
  // ============ OPTIONAL: Print stored history on boot ============
  Serial.println("\n=== Stored History ===");
  storage_print_history();
  
  // ============ CREATE TASKS ============
  xTaskCreate(taskReadScale, "taskReadScale", 2048, NULL, 1, NULL);
  xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 2, NULL);
  xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 3, NULL);
  // xTaskCreate(taskHTMLPage, "taskHTMLPage", 4096, NULL, 1, NULL);  // Enable when implementing
  // xTaskCreate(taskDailySave, "taskDailySave", 2048, NULL, 1, NULL);  // Enable for auto-save
  
  Serial.println("System initialized. Hydration tracking started.");
  
  // ============ TESTING: Example commands you can call ============
  // Uncomment these to test the storage functions:
  
  // Test 1: Save some fake data for testing
  // storage_save_today(250.0, 300.0);  // Day 1
  // delay(100);
  // storage_save_today(280.0, 300.0);  // Day 2 (simulating next day)
  
  // Test 2: Print history
  // storage_print_history();
  
  // Test 3: Get history as array
  /*
  DailyHydrationData history[STORAGE_DAYS];
  if (storage_get_history(history)) {
    Serial.println("Retrieved history:");
    for (int i = 0; i < STORAGE_DAYS; i++) {
      if (history[i].valid) {
        Serial.printf("Day %d: %.0fg/%.0fg (%.1f%%)\n", 
          history[i].timestamp,
          history[i].grams_consumed,
          history[i].daily_goal,
          (history[i].grams_consumed / history[i].daily_goal) * 100.0);
      }
    }
  }
  */
  
  // Test 4: Reset all data
  // storage_reset_all();
}

void loop() {
  // Empty - using FreeRTOS tasks
  
  // ============ OPTIONAL: Serial commands for testing ============
  // You can add serial commands here to test storage functions:
  
  if (Serial.available()) {
    char cmd = Serial.read();
    while(Serial.available()) Serial.read();  // Clear buffer
    
    switch(cmd) {
      case 's':  // Save current day
        hydration_save_day();
        Serial.println("Saved current day to storage");
        break;
        
      case 'p':  // Print history
        Serial.println("Printing history...");
        storage_print_history();
        break;
        
      case 'r':  // Reset all storage
        storage_reset_all();
        Serial.println("All storage cleared");
        break;
        
      case 'h':  // Get history as array
        {
          Serial.println("Getting history array...");
          DailyHydrationData history[STORAGE_DAYS];
          bool success = storage_get_history(history);
          
          if (success) {
            Serial.println("=== History Array ===");
            int valid_count = 0;
            for (int i = 0; i < STORAGE_DAYS; i++) {
              if (history[i].valid) {
                Serial.printf("[%d] Day=%u, Consumed=%.0fg, Goal=%.0fg, Percent=%.1f%%\n", 
                  i,
                  history[i].timestamp,
                  history[i].grams_consumed, 
                  history[i].daily_goal,
                  (history[i].grams_consumed / history[i].daily_goal) * 100.0);
                valid_count++;
              }
            }
            if (valid_count == 0) {
              Serial.println("Array retrieved but all entries are invalid (no data stored yet)");
            }
            Serial.println("====================");
          } else {
            Serial.println("Failed to get history - no data in storage");
          }
        }
        break;
        
      case 't':  // Show today's consumption
        Serial.printf("Today: %.2fg consumed out of %.2fg goal\n", 
          get_total_consumed(), DAILY_GOAL_GRAMS);
        break;
        
      case 'f':  // Fill with fake data for testing
        Serial.println("Filling storage with fake test data...");
        storage_enable_test_mode();  // Enable test mode
        for (int i = 0; i < 7; i++) {
          float consumed = 200.0 + (i * 20.0);  // 200, 220, 240, etc.
          storage_save_today(consumed, 300.0);
          Serial.printf("Added day %d: %.0fg/300g\n", i+1, consumed);
          storage_next_day();  // Move to next day
        }
        Serial.println("Fake data added! Now try 'h' or 'p'");
        break;
        
      case '?':  // Help
        Serial.println("\n=== Available Commands ===");
        Serial.println("s - Save current day to storage");
        Serial.println("p - Print history (formatted)");
        Serial.println("h - Get history as array (raw data)");
        Serial.println("r - Reset all storage");
        Serial.println("t - Show today's consumption");
        Serial.println("f - Fill with fake test data");
        Serial.println("? - Show this help");
        Serial.println("========================\n");
        break;
        
      case '\n':
      case '\r':
        // Ignore newlines
        break;
        
      default:
        Serial.printf("Unknown command: '%c'. Press '?' for help\n", cmd);
        break;
    }
  }
}