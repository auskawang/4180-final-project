#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>
#include "hydration.h"

// Configuration constants
// ESP32-C6 compatible GPIO pins - ADJUST THESE TO YOUR WIRING
#define LED_RED_PIN 6      // Connect to RED pin of RGB LED
#define LED_GREEN_PIN 7    // Connect to GREEN pin of RGB LED
#define LED_BLUE_PIN 10    // Connect to BLUE pin of RGB LED

// Set this based on your RGB LED type
#define COMMON_ANODE false  // Set to true if using common anode, false for common cathode

// PWM Configuration
#define LED_PWM_FREQ 5000
#define LED_PWM_RESOLUTION 8  // 8-bit resolution (0-255)

// LED color definitions (RGB values)
struct LEDColor {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Predefined colors for hydration states
#define COLOR_GOOD      {0, 255, 0}      // Green - well hydrated
#define COLOR_OK        {255, 255, 0}    // Yellow - acceptable
#define COLOR_LOW       {255, 128, 0}    // Orange - should drink soon
#define COLOR_CRITICAL  {255, 0, 0}      // Red - need to drink now
#define COLOR_OFF       {0, 0, 0}        // Off

// Function prototypes
void status_led_init();
void status_led_set_color(LEDColor color);
void status_led_set_from_hydration_state(HydrationState state);
void status_led_update();
void status_led_off();
void status_led_test_sequence();

#endif // STATUS_LED_H