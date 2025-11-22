#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"
#include "status_led.h"
#include "scale.h"
#include "storage.h"
#include "web.h"

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
Generates HTML page allowing user to view hydration data and interact with system wirelessly through Wifi
*/
void taskHTMLPage(void *pv) {
  while(1) {
    set_grams_left_and_tracker(read_grams_left(), read_tracker());
    set_total_grams(read_total_grams());
    webserver_handle_client();
    vTaskDelay(10);
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

    //if the tracker is 0, then that means the "day" ended, so call reset function to reset hydration values and store grams of water drank for that day
    if (read_tracker() == 0) {
      //store(read_total_grams());
      reset();
    }
      
    
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
  scale_init();
  status_led_init();
  speaker_init();
  web_init();
  xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 3, NULL);
  xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 3, NULL);
  xTaskCreate(taskReadScale, "taskReadScale", 2048, NULL, 1, NULL);
  xTaskCreate(taskHTMLPage, "taskHTMLPage", 2048, NULL, 2, NULL);
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