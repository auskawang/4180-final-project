#include "hydration.h"
#include "storage.h"
#include <Arduino.h>

HydrationState hydration_state = CRITICAL;
float initial_time = 0;
float grams_left = DAILY_GOAL_GRAMS;
float tracker = DAILY_GOAL_GRAMS;
float total_consumed = 0.0;  // Track total consumed today

void hydration_init() {
  grams_left = DAILY_GOAL_GRAMS;
  initial_time = 0;
  total_consumed = 0.0;
  Serial.println("Hydration module initialized");
}

void record_grams_drank(float grams_drank) {
  grams_left = (grams_left - grams_drank < 0) ? 0 : grams_left - grams_drank;
  total_consumed += grams_drank;
  
  Serial.printf("Recorded: %.2fg drank, Total today: %.2fg\n", grams_drank, total_consumed);
}

void update_hydration_status() {
  if (initial_time == 0) {
    initial_time = millis();
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

float get_total_consumed() {
  return total_consumed;
}

/**
 * Save today's hydration data to storage
 * Call this at end of day or when user requests
 */
void hydration_save_day() {
  storage_save_today(total_consumed, DAILY_GOAL_GRAMS);
  Serial.printf("Saved today's data: %.2fg / %.2fg\n", total_consumed, DAILY_GOAL_GRAMS);
}

/**
 * Reset current day's tracking
 * Does NOT clear 7-day history, just resets today
 */
void hydration_reset() {
  grams_left = DAILY_GOAL_GRAMS;
  initial_time = 0;
  total_consumed = 0.0;
  hydration_state = CRITICAL;
  Serial.println("Today's hydration data reset");
}