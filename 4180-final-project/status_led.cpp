#include "status_led.h"

static LEDColor current_color = COLOR_OFF;

void status_led_init() {
  // Setup RGB LED pins
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  
  // Initialize PWM for each color channel
  ledcAttach(LED_RED_PIN, LED_PWM_FREQ, LED_PWM_RESOLUTION);
  ledcAttach(LED_GREEN_PIN, LED_PWM_FREQ, LED_PWM_RESOLUTION);
  ledcAttach(LED_BLUE_PIN, LED_PWM_FREQ, LED_PWM_RESOLUTION);
  
  // Start with LED off
  status_led_off();
  
  //Serial.println("RGB LED initialized");
  //Serial.printf("LED Pins - R:%d G:%d B:%d\n", LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN);
  //Serial.printf("LED Type: %s\n", COMMON_ANODE ? "Common Anode" : "Common Cathode");
}

void status_led_set_color(LEDColor color) {
  current_color = color;
  
  // Write PWM values to each channel (adjusted for LED type)
  ledcWrite(LED_RED_PIN, color.red);
  ledcWrite(LED_GREEN_PIN, color.green);
  ledcWrite(LED_BLUE_PIN, color.blue);
  
  //Serial.printf("LED Color: R=%d G=%d B=%d\n", color.red, color.green, color.blue);
}

void status_led_set_from_hydration_state(HydrationState state) {
  LEDColor color;
  
  switch(state) {
    case HYDRATED:
      color = COLOR_GOOD;
      //Serial.println("LED:  GREEN (Good hydration)");
      break;
      
    case NEEDS_WATER:
      color = COLOR_OK;
      //Serial.println("LED:  YELLOW (OK hydration)");
      break;
      
    case CRITICAL:
      color = COLOR_CRITICAL;
      //Serial.println("LED:  RED (Critical - drink water!)");
      break;
      
    default:
      color = COLOR_OFF;
      break;
  }
  
  status_led_set_color(color);
}

void status_led_update(HydrationState state) {
  // Get current hydration state and update LED accordingly
  status_led_set_from_hydration_state(state);
}

void status_led_off() {
  LEDColor off = COLOR_OFF;
  status_led_set_color(off);
}

// void status_led_test_sequence() {
//   Serial.println("\n Testing RGB LED colors...");
  
//   // Red
//   Serial.println("Red...");
//   LEDColor red = COLOR_CRITICAL;
//   status_led_set_color(red);
//   delay(1000);
  
//   // Orange
//   Serial.println("Orange...");
//   LEDColor orange = COLOR_LOW;
//   status_led_set_color(orange);
//   delay(1000);
  
//   // Yellow
//   Serial.println("Yellow...");
//   LEDColor yellow = COLOR_OK;
//   status_led_set_color(yellow);
//   delay(1000);
  
//   // Green
//   Serial.println("Green...");
//   LEDColor green = COLOR_GOOD;
//   status_led_set_color(green);
//   delay(1000);
  
//   // Blue (bonus test color)
//   Serial.println("Blue...");
//   LEDColor blue = {0, 0, 255};
//   status_led_set_color(blue);
//   delay(1000);
  
//   // White (all colors)
//   Serial.println("White...");
//   LEDColor white = {255, 255, 255};
//   status_led_set_color(white);
//   delay(1000);
  
//   // Off
//   Serial.println("Off...");
//   status_led_off();
//   delay(500);
  
//   Serial.println("RGB LED test complete!\n");
// }