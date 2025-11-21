#include "hydration.h"

static float current_intake_ml = 0.0;
static float daily_goal_ml = HYDRATION_GOAL_ML;
static HydrationState current_state = HYDRATION_CRITICAL;

void hydration_init() {
  current_intake_ml = 0.0;
  daily_goal_ml = HYDRATION_GOAL_ML;
  current_state = HYDRATION_CRITICAL;
  Serial.println("Hydration tracker initialized");
  Serial.printf("Daily goal: %.0f ml\n", daily_goal_ml);
}

void hydration_update_from_weight(float delta_grams) {
  // Negative delta means water was consumed
  if (delta_grams < -5.0) {  // Threshold to avoid noise
    float consumed_ml = -delta_grams;  // 1 gram = 1 ml for water
    hydration_add_intake(consumed_ml);
  }
}

void hydration_add_intake(float ml) {
  current_intake_ml += ml;
  
  Serial.printf("Added %.0f ml. Total: %.0f ml / %.0f ml\n", 
                ml, current_intake_ml, daily_goal_ml);
  
  // Update state based on current intake
  if (current_intake_ml >= OK_THRESHOLD_ML) {
    current_state = HYDRATION_GOOD;
  } else if (current_intake_ml >= LOW_THRESHOLD_ML) {
    current_state = HYDRATION_OK;
  } else if (current_intake_ml >= CRITICAL_THRESHOLD_ML) {
    current_state = HYDRATION_LOW;
  } else {
    current_state = HYDRATION_CRITICAL;
  }
  
  const char* state_names[] = {"GOOD", "OK", "LOW", "CRITICAL"};
  Serial.printf("Hydration state: %s\n", state_names[current_state]);
}

void hydration_reset_daily() {
  current_intake_ml = 0.0;
  current_state = HYDRATION_CRITICAL;
  Serial.println("Daily hydration reset");
}

HydrationState hydration_get_state() {
  return current_state;
}

float hydration_get_current_ml() {
  return current_intake_ml;
}

float hydration_get_goal_ml() {
  return daily_goal_ml;
}

int hydration_get_percentage() {
  return (int)((current_intake_ml / daily_goal_ml) * 100.0);
}

bool hydration_needs_alert() {
  return (current_state == HYDRATION_CRITICAL || current_state == HYDRATION_LOW);
}