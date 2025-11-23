#ifndef POWER_H
#define POWER_H

#include <Arduino.h>
#include "config.h"

// Power management functions
void power_init();
void power_update_activity();
bool power_should_sleep();
void power_sleep_indicator();
void power_enter_sleep();
void power_check_and_sleep();

#endif