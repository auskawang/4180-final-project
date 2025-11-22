#ifndef HYDRATION_H
#define HYDRATION_H
#include "config.h"

void update_hydration_status();
HydrationState get_hydration_state();
void record_grams_drank(float grams_drank);
float read_grams_left();
float read_tracker();
void reset();
float read_total_grams();
#endif
