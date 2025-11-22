#include "hydration.h"
#include <Arduino.h>
HydrationState hydration_state = HYDRATED;
float initial_time = 0;
const float goal_grams = 300.0;
float grams_left = goal_grams;
float total_time_ms = 120 * 1000;
float tracker = goal_grams;
void update_hydration_status(float grams_drank) {
  if (initial_time == 0) {
    initial_time = millis();
  }
  float time_now = millis();
  float time_since_start = time_now - initial_time;
  
  //tracker should decrease linearly (goal - goal/total_time * ms passed)
  float tracker = (tracker > 300) ? 300 : goal_grams/total_time_ms * time_since_start;
  //Serial.println(goal_grams - tracker);

  //record meaningful changes
  if (grams_drank < 30.0) {
    grams_drank = 0;
  }

  grams_left = (grams_left - grams_drank < 0) ? 0 : grams_left - grams_drank;

  //if we are behind track then change hydration state to CRITICAL
  if (goal_grams - tracker - grams_left < 0) {
    hydration_state = CRITICAL;
  }
  //if tracker is close then set to NEEDS_WATER
  else if (goal_grams - tracker - grams_left < goal_grams / 5) {
    hydration_state = NEEDS_WATER;
  }
  else {
    hydration_state = HYDRATED;
  }
}

HydrationState get_hydration_state() {
  return hydration_state;
}