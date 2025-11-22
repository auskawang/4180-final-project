#include "hydration.h"
#include "storage.h"
#include <Arduino.h>

HydrationState hydration_state = CRITICAL;
float initial_time = 0;
float grams_left = DAILY_GOAL_GRAMS;
float tracker = DAILY_GOAL_GRAMS;

void hydration_init() {
  // Try to load previous data from storage
  HydrationData stored_data;
  if (storage_load_hydration_data(&stored_data)) {
    grams_left = stored_data.grams_left;
    initial_time = stored_data.initial_time;
    Serial.println("Loaded hydration data from storage");
  } else {
    // No stored data, use defaults
    grams_left = DAILY_GOAL_GRAMS;
    initial_time = 0;
    Serial.println("Starting with fresh hydration data");
  }
}

void record_grams_drank(float grams_drank) {
  grams_left = (grams_left - grams_drank < 0) ? 0 : grams_left - grams_drank;
  
  // Save to storage whenever consumption is recorded
  storage_save_hydration_data(grams_left, initial_time, DAILY_GOAL_GRAMS);
}

void update_hydration_status() {
  if (initial_time == 0) {
    initial_time = millis();
    // Save initial time to storage
    storage_save_hydration_data(grams_left, initial_time, DAILY_GOAL_GRAMS);
  }
  
  float time_now = millis();
  float time_since_start = time_now - initial_time;
  
  // tracker mimics the ideal grams_left at a certain time
  float subtract = DAILY_GOAL_GRAMS/TOTAL_TIME_MS * time_since_start;
  float tracker = (DAILY_GOAL_GRAMS - subtract < 0) ? 0 : DAILY_GOAL_GRAMS - subtract;

  Serial.print("grams_left=");
  Serial.print(grams_left);
  Serial.print(" tracker=");
  Serial.println(tracker);

  // Update hydration state based on progress
  if (tracker - grams_left < 0) {
    hydration_state = CRITICAL;
  }
  else if (tracker - grams_left < DAILY_GOAL_GRAMS / 5) {
    hydration_state = NEEDS_WATER;
  }
  else {
    hydration_state = HYDRATED;
  }
}

HydrationState get_hydration_state() {
  return hydration_state;
}

void hydration_reset() {
  grams_left = DAILY_GOAL_GRAMS;
  initial_time = 0;
  hydration_state = CRITICAL;
  storage_clear_all();
  Serial.println("Hydration data reset");
}