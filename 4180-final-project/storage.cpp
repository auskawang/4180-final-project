#include "storage.h"
#include <Preferences.h>

static Preferences prefs;
static bool initialized = false;

/**
 * Initialize NVS storage
 * Must be called before any other storage functions
 */
bool storage_init() {
  if (initialized) {
    return true;
  }
  
  // No need to call nvs_flash_init() manually - Preferences handles it
  initialized = true;
  Serial.println("Storage initialized");
  return true;
}

/**
 * Save hydration data to NVS
 * This is the main function you'll call from your main code
 */
bool storage_save_hydration_data(float grams_left, float initial_time, float daily_goal) {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return false;
  }
  
  // Open preferences in read-write mode
  if (!prefs.begin(NVS_NAMESPACE, false)) {
    Serial.println("Error: Failed to open NVS namespace");
    return false;
  }
  
  // Save each value
  prefs.putFloat(KEY_GRAMS_LEFT, grams_left);
  prefs.putFloat(KEY_INITIAL_TIME, initial_time);
  prefs.putFloat(KEY_DAILY_GOAL, daily_goal);
  
  // Close preferences
  prefs.end();
  
  Serial.printf("Saved: grams_left=%.2f, initial_time=%.2f, daily_goal=%.2f\n", 
                grams_left, initial_time, daily_goal);
  return true;
}

/**
 * Load hydration data from NVS
 * Returns true if data was found, false otherwise
 */
bool storage_load_hydration_data(HydrationData* data) {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return false;
  }
  
  if (data == NULL) {
    Serial.println("Error: NULL pointer provided");
    return false;
  }
  
  // Open preferences in read-only mode
  if (!prefs.begin(NVS_NAMESPACE, true)) {
    Serial.println("Error: Failed to open NVS namespace");
    return false;
  }
  
  // Check if data exists
  if (!prefs.isKey(KEY_GRAMS_LEFT)) {
    Serial.println("No saved data found");
    prefs.end();
    return false;
  }
  
  // Load each value
  data->grams_left = prefs.getFloat(KEY_GRAMS_LEFT, 0.0);
  data->initial_time = prefs.getFloat(KEY_INITIAL_TIME, 0.0);
  data->daily_goal = prefs.getFloat(KEY_DAILY_GOAL, 300.0);
  
  // Close preferences
  prefs.end();
  
  Serial.printf("Loaded: grams_left=%.2f, initial_time=%.2f, daily_goal=%.2f\n",
                data->grams_left, data->initial_time, data->daily_goal);
  return true;
}

/**
 * Clear all stored hydration data
 * Useful for reset functionality
 */
bool storage_clear_all() {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return false;
  }
  
  if (!prefs.begin(NVS_NAMESPACE, false)) {
    Serial.println("Error: Failed to open NVS namespace");
    return false;
  }
  
  prefs.clear();
  prefs.end();
  
  Serial.println("All storage data cleared");
  return true;
}

/**
 * Debug function to print current stored values
 */
void storage_print_data() {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return;
  }
  
  HydrationData data;
  if (storage_load_hydration_data(&data)) {
    Serial.println("=== Stored Hydration Data ===");
    Serial.printf("Grams Left: %.2f\n", data.grams_left);
    Serial.printf("Initial Time: %.2f\n", data.initial_time);
    Serial.printf("Daily Goal: %.2f\n", data.daily_goal);
    Serial.println("============================");
  } else {
    Serial.println("No data in storage");
  }
}