#ifndef HYDRATION_H
#define HYDRATION_H
#include "config.h"

void hydration_init();
void update_hydration_status();
HydrationState get_hydration_state();
void record_grams_drank(float grams_drank);
void hydration_reset();

#endif