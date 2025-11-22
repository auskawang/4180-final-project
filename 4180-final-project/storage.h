#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

// Storage namespace for NVS
#define NVS_NAMESPACE "hydration"

// Keys for NVS storage
#define KEY_GRAMS_LEFT "grams_left"
#define KEY_INITIAL_TIME "init_time"
#define KEY_DAILY_GOAL "daily_goal"

// Structure to hold hydration data
struct HydrationData {
  float grams_left;
  float initial_time;
  float daily_goal;
};

// Function prototypes
bool storage_init();
bool storage_save_hydration_data(float grams_left, float initial_time, float daily_goal);
bool storage_load_hydration_data(HydrationData* data);
bool storage_clear_all();
void storage_print_data();

#endif // STORAGE_H