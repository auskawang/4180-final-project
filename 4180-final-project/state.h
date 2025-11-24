#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "config.h"

void change_to_running();
SystemState get_state();

extern SystemState current_state;

#endif 