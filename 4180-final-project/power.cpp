#include "power.h"
#include "status_led.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

// Sleep configuration
#define INACTIVITY_TIMEOUT_MS 20000  // 20 seconds of no weight change
#define SLEEP_DURATION_SEC 15       // Wake up every 15 seconds to check

static unsigned long last_activity_time = 0;
static bool activity_detected = false;

void power_init() {
  last_activity_time = millis();
  activity_detected = false;
  
  // Configure wake-up source - wake up periodically to check scale
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SEC * 1000000ULL); // microseconds
  
  Serial.println("Power manager initialized");
  Serial.printf("Inactivity timeout: %d ms\n", INACTIVITY_TIMEOUT_MS);
}

void power_update_activity() {
  // Call this whenever meaningful weight change is detected
  last_activity_time = millis();
  activity_detected = true;
  
  Serial.println("Activity detected - resetting sleep timer");
}

bool power_should_sleep() {
  unsigned long current_time = millis();
  unsigned long time_since_activity = current_time - last_activity_time;
  
  // Check if we've been inactive long enough
  if (time_since_activity >= INACTIVITY_TIMEOUT_MS) {
    Serial.printf("Inactive for %lu ms - ready to sleep\n", time_since_activity);
    return true;
  }
  
  return false;
}

void power_sleep_indicator() {
  // Visual indication that we're about to sleep
  Serial.println("Sleep indicator: Blinking LED");
  
  LEDColor blue = {0, 0, 255};  // Blue indicates sleep mode
  LEDColor off = COLOR_OFF;
  
  // Blink 3 times fast
  for (int i = 0; i < 3; i++) {
    status_led_set_color(blue);
    delay(200);
    status_led_set_color(off);
    delay(200);
  }
  
  // Turn off LED completely before sleep to save power
  status_led_off();
}

void power_enter_sleep() {
  Serial.println("Entering deep sleep mode...");
  Serial.printf("Will wake up in %d seconds\n", SLEEP_DURATION_SEC);
  
  // Show visual indication
  power_sleep_indicator();
  
  Serial.println("Going to sleep NOW...");
  Serial.flush();
  
  // Small delay to ensure everything completes
  delay(100);
  
  // Enter deep sleep
  esp_deep_sleep_start();
}

void power_check_and_sleep() {
  // This function should be called periodically (e.g., in taskPowerManager)
  if (power_should_sleep()) {
    power_enter_sleep();
  }
}
