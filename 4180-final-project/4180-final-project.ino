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
    // HTML server logic - TO BE IMPLEMENTED
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Detects scale changes and stores meaningful changes in non-volatile memory
*/
void taskReadScale(void *pv) {
  while(1) {
    // Scale reading logic - TO BE IMPLEMENTED
    // Example: 
    // float weight_change = scale_read_grams();
    // hydration_update_from_weight(weight_change);
    
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
  delay(1000);  // Give serial time to initialize
  
  Serial.println("\n\n╔════════════════════════════════════╗");
  Serial.println("║   HYDRATION TRACKER STARTING...   ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // Initialize modules
  Serial.println("Initializing modules...");
  hydration_init();
  speaker_init();
  status_led_init();
  
  // Run LED test sequence on startup
  Serial.println("\n🎨 Running LED startup test...");
  status_led_test_sequence();
  
  Serial.println("\nCreating FreeRTOS tasks...");
  
  // Create FreeRTOS tasks
  xTaskCreate(taskUpdateStatusLED, "StatusLED", 2048, NULL, 1, NULL);
  xTaskCreate(taskAlertUser, "AlertUser", 2048, NULL, 2, NULL);
  xTaskCreate(taskTestHydration, "TestHydration", 2048, NULL, 1, NULL);
  
  // Uncomment these as you implement them:
  // xTaskCreate(taskReadScale, "ReadScale", 4096, NULL, 2, NULL);
  // xTaskCreate(taskHTMLPage, "HTMLPage", 8192, NULL, 1, NULL);
  // xTaskCreate(taskPowerManager, "PowerManager", 2048, NULL, 1, NULL);
  
  Serial.println("✓ All tasks created successfully\n");
  
  Serial.println("╔════════════════════════════════════╗");
  Serial.println("║    HYDRATION TRACKER ACTIVE!      ║");
  Serial.println("╚════════════════════════════════════╝");
  Serial.println("\n🚰 System Status:");
  Serial.println("  🔴 LED: RED (CRITICAL state)");
  Serial.println("  🔊 Speaker: Alerts every 30 seconds");
  Serial.println("  💧 Hydration: 0ml / 2000ml (0%)");
  Serial.println("\n💡 TIP: Type 'h' in Serial Monitor for commands\n");
  Serial.println("─────────────────────────────────────\n");
}

void loop() {
  static bool led_on = true;
  
  // Handle serial commands for testing
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // Clear any trailing newline/carriage return
    while(Serial.available() > 0 && (Serial.peek() == '\n' || Serial.peek() == '\r')) {
      Serial.read();
    }
    
    switch(cmd) {
      case 'd':  // Drink 200ml
        Serial.println("\n💧 Simulating drinking 200ml...");
        hydration_add_intake(200);
        Serial.printf("Progress: %d%% (%.0f / %.0f ml)\n", 
                     hydration_get_percentage(),
                     hydration_get_current_ml(),
                     hydration_get_goal_ml());
        status_led_update();  // Immediately update LED
        break;
        
      case 'D':  // Drink 500ml (bigger gulp)
        Serial.println("\n💧💧 Simulating drinking 500ml...");
        hydration_add_intake(500);
        Serial.printf("Progress: %d%% (%.0f / %.0f ml)\n", 
                     hydration_get_percentage(),
                     hydration_get_current_ml(),
                     hydration_get_goal_ml());
        status_led_update();  // Immediately update LED
        break;
        
      case 'r':
      case 'R':
        Serial.println("\n🔄 Resetting daily hydration...");
        hydration_reset_daily();
        status_led_update();  // Immediately update LED
        Serial.println("✓ Reset complete - back to CRITICAL state");
        break;
        
      case 's':
      case 'S':
        Serial.println("\n╔════════ CURRENT STATUS ════════╗");
        Serial.printf("  Intake: %.0f ml / %.0f ml\n", 
                     hydration_get_current_ml(), 
                     hydration_get_goal_ml());
        Serial.printf("  Progress: %d%%\n", hydration_get_percentage());
        Serial.print("  State: ");
        switch(hydration_get_state()) {
          case HYDRATION_GOOD: 
            Serial.println("✅ GOOD (Green LED)"); 
            break;
          case HYDRATION_OK: 
            Serial.println("💛 OK (Yellow LED)"); 
            break;
          case HYDRATION_LOW: 
            Serial.println("🟠 LOW (Orange LED)"); 
            break;
          case HYDRATION_CRITICAL: 
            Serial.println("🔴 CRITICAL (Red LED)"); 
            break;
        }
        Serial.printf("  Alerts: %s\n", alert_enabled ? "🔊 ENABLED" : "🔇 DISABLED");
        Serial.printf("  LED: %s\n", led_on ? "💡 ON" : "⚫ OFF");
        Serial.println("╚════════════════════════════════╝");
        break;
        
      case 'a':
      case 'A':
        alert_enabled = !alert_enabled;
        Serial.printf("\n🔔 Alerts %s\n", alert_enabled ? "🔊 ENABLED" : "🔇 DISABLED");
        if (!alert_enabled) {
          Serial.println("   (Speaker will be silent)");
        }
        break;
        
      case 't':
      case 'T':
        Serial.println("\n🎨 Running LED test sequence...");
        status_led_test_sequence();
        status_led_update();  // Return to current hydration state
        break;
        
      case 'l':
      case 'L':
        led_on = !led_on;
        if (led_on) {
          Serial.println("\n💡 LED ON");
          status_led_update();
        } else {
          Serial.println("\n💡 LED OFF");
          status_led_off();
        }
        break;
        
      case 'h':
      case 'H':
        Serial.println("\n========== HELP MENU ==========");
        Serial.println("Available Commands:");
        Serial.println("  'd' - Add 200ml to hydration");
        Serial.println("  'D' - Add 500ml to hydration");
        Serial.println("  'r' - Reset daily hydration");
        Serial.println("  's' - Show detailed status");
        Serial.println("  'a' - Toggle speaker alerts");
        Serial.println("  't' - Test LED colors");
        Serial.println("  'l' - Toggle LED on/off");
        Serial.println("  'h' - Show this help");
        Serial.println("\nHydration Levels:");
        Serial.println("  🔴 RED    = 0-500ml   (CRITICAL)");
        Serial.println("  🟠 ORANGE = 500-1000ml (LOW)");
        Serial.println("  💛 YELLOW = 1000-1500ml (OK)");
        Serial.println("  💚 GREEN  = 1500ml+   (GOOD)");
        Serial.println("===============================\n");
        break;
        
      case '\n':
      case '\r':
        // Ignore newlines/carriage returns
        break;
        
      default:
        Serial.printf("\n❌ Unknown command: '%c'\n", cmd);
        Serial.println("Type 'h' for help\n");
        break;
    }
  }
  
  delay(100);
}