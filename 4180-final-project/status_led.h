#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>
#include "config.h"

// Configuration constants
// ESP32-C6 compatible GPIO pins - ADJUST THESE TO YOUR WIRING


// Set this based on your RGB LED type


// Function prototypes
void status_led_init();
void status_led_set_color(LEDColor color);
void status_led_set_from_hydration_state(HydrationState state);
void status_led_update(HydrationState state);
void status_led_off();
//void status_led_test_sequence();

#endif // STATUS_LED_H