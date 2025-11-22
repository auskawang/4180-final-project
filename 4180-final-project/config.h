#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//SCALE -----------------------------------------------------------
#define SCALE_DATA_PIN 6
#define SCALE_CLK_PIN 7
#define SCALE_CALIBRATION_VAL -108.207550

//HYDRATION -----------------------------------------------------------
#define DAILY_GOAL_GRAMS 300.0
#define TOTAL_TIME_MS (30.0 * 1000) //120 seconds

enum HydrationState {
    HYDRATED,
    NEEDS_WATER,
    CRITICAL
};

//SPEAKER -----------------------------------------------------------
#define SPEAKER_PIN 21
#define ALERT_FREQUENCY 1000  // Hz
#define ALERT_DURATION 500    // ms
#define ALERT_INTERVAL 30000  // 30 seconds between alerts

enum SpeakerState {
  SPEAKER_IDLE,
  SPEAKER_ALERTING
};

//STATUS LED -----------------------------------------------------------
#define LED_RED_PIN 15
#define LED_GREEN_PIN 23
#define LED_BLUE_PIN 22

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


#endif