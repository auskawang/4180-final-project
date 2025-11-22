#ifndef HYDRATION_H
#define HYDRATION_H
#include "config.h"
#include "storage.h"

void hydration_init();
void update_hydration_status();
HydrationState get_hydration_state();
void record_grams_drank(float grams_drank);
void hydration_save_day();  // Save today's data to storage
void hydration_reset();      // Reset current day
float get_total_consumed();  // Get total consumed today

#endif