#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//DEBUG ---------
#define DEBUG 1

//SCALE -----------------------------------------------------------
#define SCALE_DATA_PIN 6
#define SCALE_CLK_PIN 7
#define SCALE_CALIBRATION_VAL -256.602600

//HYDRATION -----------------------------------------------------------
#define DAILY_GOAL_GRAMS 300.0
#define TOTAL_TIME_MS (10.0 * 1000) //120 seconds

enum HydrationState {
    COMPLETED,
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
#define LED_PIN 8
#define NUM_LEDS 1
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
#define COLOR_WHITE       {128, 128, 128}    // Orange - should drink soon
#define COLOR_CRITICAL    {255, 0, 0}      // Red - nice!
#define COLOR_HOORAY    {255, 255, 255}      // White - nice!
#define COLOR_OFF       {0, 0, 0}        // Off

//STORAGE -----------------------------------------------------------
// Number of days to store
#define MAX_ENTRIES 7

// Structure to hold a single day's hydration data
struct Entry {
  uint32_t timestamp;        // Unix timestamp for the day
  float grams_drank;      // Total grams consumed that day
  float goal;          // Daily goal for that day
};

//WEB -----------------------------------------------------------
#define BTN_PIN 5
#define WEB_STATUS_PIN 4
#define CLIENT_TIMEOUT_SECS 30

//STATE -----------------------------------------------------------
typedef enum {
    STATE_WAITING_USER_INPUT,
    STATE_RUNNING
} SystemState;

#endif