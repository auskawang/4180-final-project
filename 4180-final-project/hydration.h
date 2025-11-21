#ifndef HYDRATION_H
#define HYDRATION_H

#include <Arduino.h>

// Hydration states
enum HydrationState {
  HYDRATION_GOOD,      // Well hydrated
  HYDRATION_OK,        // Acceptable level
  HYDRATION_LOW,       // Should drink soon
  HYDRATION_CRITICAL   // Need to drink now (triggers alert)
};

// Configuration
#define HYDRATION_GOAL_ML 2000          // Daily goal in milliliters
#define CRITICAL_THRESHOLD_ML 500       // Below this triggers alerts
#define LOW_THRESHOLD_ML 1000           // Below this shows warning
#define OK_THRESHOLD_ML 1500            // Below this shows OK status

// Function prototypes
void hydration_init();
void hydration_update_from_weight(float delta_grams);
void hydration_add_intake(float ml);
void hydration_reset_daily();
HydrationState hydration_get_state();
float hydration_get_current_ml();
float hydration_get_goal_ml();
int hydration_get_percentage();
bool hydration_needs_alert();

#endif // HYDRATION_H