#ifndef HYDRATION_H
#define HYDRATION_H
#include "config.h"
#include "storage.h"

void hydration_init();
void update_hydration_status();
HydrationState get_hydration_state();
void record_grams_drank(float grams_drank);
float read_grams_left();
float read_tracker();
float read_goal_grams();
void reset();
float read_total_grams();
void set_goal(int goal);
void set_time_length(int seconds);
int read_time_length();
#endif
