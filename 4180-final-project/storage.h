#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

// Storage namespace for NVS
#define NVS_NAMESPACE "hydration"

// Keys for NVS storage
#define KEY_DAY_PREFIX "day_"
#define KEY_DAY_COUNT "day_count"

// Number of days to store
#define STORAGE_DAYS 7

// Structure to hold a single day's hydration data
struct DailyHydrationData {
  uint32_t timestamp;        // Unix timestamp for the day
  float grams_consumed;      // Total grams consumed that day
  float daily_goal;          // Daily goal for that day
  bool valid;                // Is this entry valid?
};

// Function prototypes
bool storage_init();
bool storage_save_today(float grams_consumed, float daily_goal);
bool storage_get_history(DailyHydrationData history[STORAGE_DAYS]);
bool storage_reset_all();
void storage_print_history();

// Testing functions
void storage_enable_test_mode();  // Enable manual day simulation
void storage_next_day();           // Advance to next day (test mode only)

#endif // STORAGE_H