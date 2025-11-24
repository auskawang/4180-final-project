#include "hydration.h"
#include "storage.h"
#include <Arduino.h>

HydrationState hydration_state = CRITICAL;
float initial_time = 0;
float goal = 0;
float grams_left = DAILY_GOAL_GRAMS;
float tracker = DAILY_GOAL_GRAMS;
float total_grams = 0;
unsigned long time_period_ms = 0;
void record_grams_drank(float grams_drank) {
  grams_left = (grams_left - grams_drank < 0) ? 0 : grams_left - grams_drank;
  total_grams += grams_drank;
}
void reset() {
  initial_time = 0;
  //grams_left = DAILY_GOAL_GRAMS;
  tracker = read_goal_grams();
  goal = read_goal_grams();
  grams_left = read_goal_grams();
  total_grams = 0;
  //time_period_ms = 0;
}

void update_hydration_status() {
  if (initial_time == 0) {
    initial_time = millis();
  }
  
  float time_now = millis();
  float time_since_start = time_now - initial_time;
  
  // tracker mimics the ideal grams_left at a certain time
  float subtract = goal/time_period_ms * time_since_start;
  tracker = (goal - subtract < 0) ? 0 : goal - subtract;

  if (DEBUG) {
    Serial.print("grams_left=");
    Serial.print(grams_left);
    Serial.print(" tracker=");
    Serial.println(tracker);
  }

  //if met goal, congrats!
  if (grams_left == 0) {
    hydration_state = COMPLETED;
  }
  else {
    // Update hydration state based on progress
    if (tracker - grams_left < 0) {
      hydration_state = CRITICAL;
    }
    else if (tracker - grams_left < goal / 5) {
      hydration_state = NEEDS_WATER;
    }
    else {
      hydration_state = HYDRATED;
    }
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
  return goal - grams_left;
}

float read_goal_grams() {
  return goal;
}

void set_goal(int goal_param) {
  goal = goal_param;
}

void set_time_length(int seconds) {
  time_period_ms = seconds * 1000;
}

int read_time_length() {
  return time_period_ms;
}