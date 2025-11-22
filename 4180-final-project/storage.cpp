#include "storage.h"
#include <Preferences.h>
#include <time.h>

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
  
  initialized = true;
  Serial.println("Storage initialized - 7 day circular buffer");
  return true;
}

// For testing: manual day counter
static uint32_t manual_day_counter = 0;
static bool use_manual_day = false;

/**
 * Get today's date as a simple day number (days since epoch)
 * This helps identify unique days
 */
static uint32_t get_day_number() {
  if (use_manual_day) {
    return manual_day_counter;
  }
  // For embedded systems without RTC, use millis() / milliseconds_per_day
  // In production, you'd use actual RTC time
  return millis() / (1000UL * 60 * 60 * 24);  // Days since boot
}

/**
 * Enable manual day mode for testing
 * Allows you to simulate different days without waiting 24 hours
 */
void storage_enable_test_mode() {
  use_manual_day = true;
  manual_day_counter = 0;
  Serial.println("Storage test mode enabled - use storage_next_day() to simulate days");
}

/**
 * Increment the day counter (for testing only)
 */
void storage_next_day() {
  if (use_manual_day) {
    manual_day_counter++;
    Serial.printf("Simulated day advanced to: %u\n", manual_day_counter);
  }
}

/**
 * Save today's hydration data
 * Automatically handles circular buffer - removes oldest if needed
 */
bool storage_save_today(float grams_consumed, float daily_goal) {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return false;
  }
  
  if (!prefs.begin(NVS_NAMESPACE, false)) {
    Serial.println("Error: Failed to open NVS namespace");
    return false;
  }
  
  // Get current day count (how many days stored)
  uint8_t day_count = prefs.getUChar(KEY_DAY_COUNT, 0);
  
  uint32_t today = get_day_number();
  
  // Check if today already exists in storage (update case)
  bool found_today = false;
  uint8_t update_index = 0;
  
  for (uint8_t i = 0; i < day_count && i < STORAGE_DAYS; i++) {
    char key[16];
    snprintf(key, sizeof(key), "%s%d_ts", KEY_DAY_PREFIX, i);
    uint32_t stored_day = prefs.getUInt(key, 0);
    
    if (stored_day == today) {
      found_today = true;
      update_index = i;
      break;
    }
  }
  
  uint8_t save_index;
  
  if (found_today) {
    // Update existing entry
    save_index = update_index;
    Serial.printf("Updating existing entry for day %d at index %d\n", today, save_index);
  } else {
    // New day - check if buffer is full
    if (day_count >= STORAGE_DAYS) {
      // Buffer full - shift all entries down (remove oldest)
      Serial.println("Buffer full - removing oldest entry");
      
      for (uint8_t i = 0; i < STORAGE_DAYS - 1; i++) {
        // Copy day i+1 to day i
        char ts_key_src[16], ts_key_dst[16];
        char gc_key_src[16], gc_key_dst[16];
        char dg_key_src[16], dg_key_dst[16];
        
        snprintf(ts_key_src, sizeof(ts_key_src), "%s%d_ts", KEY_DAY_PREFIX, i + 1);
        snprintf(ts_key_dst, sizeof(ts_key_dst), "%s%d_ts", KEY_DAY_PREFIX, i);
        snprintf(gc_key_src, sizeof(gc_key_src), "%s%d_gc", KEY_DAY_PREFIX, i + 1);
        snprintf(gc_key_dst, sizeof(gc_key_dst), "%s%d_gc", KEY_DAY_PREFIX, i);
        snprintf(dg_key_src, sizeof(dg_key_src), "%s%d_dg", KEY_DAY_PREFIX, i + 1);
        snprintf(dg_key_dst, sizeof(dg_key_dst), "%s%d_dg", KEY_DAY_PREFIX, i);
        
        prefs.putUInt(ts_key_dst, prefs.getUInt(ts_key_src, 0));
        prefs.putFloat(gc_key_dst, prefs.getFloat(gc_key_src, 0.0));
        prefs.putFloat(dg_key_dst, prefs.getFloat(dg_key_src, 0.0));
      }
      
      save_index = STORAGE_DAYS - 1;
    } else {
      // Buffer not full - add to end
      save_index = day_count;
      day_count++;
      prefs.putUChar(KEY_DAY_COUNT, day_count);
    }
    
    Serial.printf("Saving new entry for day %d at index %d\n", today, save_index);
  }
  
  // Save the data at save_index
  char ts_key[16], gc_key[16], dg_key[16];
  snprintf(ts_key, sizeof(ts_key), "%s%d_ts", KEY_DAY_PREFIX, save_index);
  snprintf(gc_key, sizeof(gc_key), "%s%d_gc", KEY_DAY_PREFIX, save_index);
  snprintf(dg_key, sizeof(dg_key), "%s%d_dg", KEY_DAY_PREFIX, save_index);
  
  prefs.putUInt(ts_key, today);
  prefs.putFloat(gc_key, grams_consumed);
  prefs.putFloat(dg_key, daily_goal);
  
  prefs.end();
  
  Serial.printf("Saved: day=%d, consumed=%.2fg, goal=%.2fg\n", 
                today, grams_consumed, daily_goal);
  return true;
}

/**
 * Get the entire 7-day history as an array
 * Returns data in chronological order (oldest to newest)
 * Array must be size STORAGE_DAYS (7)
 */
bool storage_get_history(DailyHydrationData history[STORAGE_DAYS]) {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return false;
  }
  
  // Initialize all entries as invalid
  for (uint8_t i = 0; i < STORAGE_DAYS; i++) {
    history[i].valid = false;
    history[i].timestamp = 0;
    history[i].grams_consumed = 0.0;
    history[i].daily_goal = 0.0;
  }
  
  if (!prefs.begin(NVS_NAMESPACE, true)) {
    Serial.println("Error: Failed to open NVS namespace");
    return false;
  }
  
  uint8_t day_count = prefs.getUChar(KEY_DAY_COUNT, 0);
  
  if (day_count == 0) {
    prefs.end();
    Serial.println("No history data found");
    return false;
  }
  
  // Read all stored days
  for (uint8_t i = 0; i < day_count && i < STORAGE_DAYS; i++) {
    char ts_key[16], gc_key[16], dg_key[16];
    snprintf(ts_key, sizeof(ts_key), "%s%d_ts", KEY_DAY_PREFIX, i);
    snprintf(gc_key, sizeof(gc_key), "%s%d_gc", KEY_DAY_PREFIX, i);
    snprintf(dg_key, sizeof(dg_key), "%s%d_dg", KEY_DAY_PREFIX, i);
    
    history[i].timestamp = prefs.getUInt(ts_key, 0);
    history[i].grams_consumed = prefs.getFloat(gc_key, 0.0);
    history[i].daily_goal = prefs.getFloat(dg_key, 0.0);
    history[i].valid = (history[i].timestamp != 0);
  }
  
  prefs.end();
  
  Serial.printf("Retrieved %d days of history\n", day_count);
  return true;
}

/**
 * Clear all stored hydration data
 * Resets the entire 7-day buffer
 */
bool storage_reset_all() {
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
  
  Serial.println("All 7-day history cleared");
  return true;
}

/**
 * Debug function to print the entire history
 */
void storage_print_history() {
  if (!initialized) {
    Serial.println("Error: Storage not initialized");
    return;
  }
  
  DailyHydrationData history[STORAGE_DAYS];
  
  if (storage_get_history(history)) {
    Serial.println("=== 7-Day Hydration History ===");
    Serial.println("Day | Consumed | Goal | %Complete");
    Serial.println("----+----------+------+----------");
    
    for (uint8_t i = 0; i < STORAGE_DAYS; i++) {
      if (history[i].valid) {
        float percent = (history[i].grams_consumed / history[i].daily_goal) * 100.0;
        Serial.printf(" %2d | %6.0fg | %4.0fg | %6.1f%%\n", 
                      history[i].timestamp,
                      history[i].grams_consumed,
                      history[i].daily_goal,
                      percent);
      }
    }
    Serial.println("==============================");
  } else {
    Serial.println("No history to display");
  }
}