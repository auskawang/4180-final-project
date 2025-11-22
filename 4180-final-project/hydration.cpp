#include "hydration.h"
#include <Arduino.h>

HydrationState hydration_state = CRITICAL;
float initial_time = 0;
float grams_left = DAILY_GOAL_GRAMS;
float tracker = DAILY_GOAL_GRAMS;
float total_grams = 0;

void record_grams_drank(float grams_drank) {
  grams_left = (grams_left - grams_drank < 0) ? 0 : grams_left - grams_drank;
  total_grams += grams_drank;
}
void reset() {
  initial_time = 0;
  grams_left = DAILY_GOAL_GRAMS;
  tracker = DAILY_GOAL_GRAMS;
  total_grams = 0;
}
void update_hydration_status() {
  if (initial_time == 0) {
    initial_time = millis();
  }
  float time_now = millis();
  float time_since_start = time_now - initial_time;
  
  //tracker mimics the ideal grams_left at a certain time; tracker should decrease linearly (goal - goal/total_time * ms passed)
  float subtract = DAILY_GOAL_GRAMS/TOTAL_TIME_MS * time_since_start;
  tracker = (DAILY_GOAL_GRAMS - subtract < 0) ? 0 : DAILY_GOAL_GRAMS - subtract;

  Serial.print("grams_left=");
  Serial.print(grams_left);
  Serial.print(" tracker=");
  Serial.println(tracker);

  //if we are behind track then change hydration state to CRITICAL
  if (tracker - grams_left < 0) {
    hydration_state = CRITICAL;
  }
  //if tracker is close then set to NEEDS_WATER
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

float read_grams_left() {
  return grams_left;
}

float read_tracker() {
  return tracker;
}

float read_total_grams() {
  return DAILY_GOAL_GRAMS - grams_left;
}