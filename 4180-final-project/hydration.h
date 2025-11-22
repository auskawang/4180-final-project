#ifndef HYDRATION_H
#define HYDRATION_H
#include "config.h"

void update_hydration_status();
HydrationState get_hydration_state();
void record_grams_drank(float grams_drank);
#endif
