#include "status_led.h"

static LEDColor current_color = COLOR_OFF;
Adafruit_NeoPixel rgb(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void status_led_init() {
  rgb.begin();
  rgb.show();
}

void status_led_show_waiting() {
  rgb.setPixelColor(0, rgb.Color(120, 120, 120));
  rgb.show();
}
void status_led_set_color(LEDColor color) {
    static uint16_t hue = 0;        // hue in HSV space (0-65535)
    static bool rainbow_done = false;
    static uint8_t fade_level = 255; // for fading to off

    if (color.red == 255 && color.green == 255 && color.blue == 255) {
        if (!rainbow_done) {
            // Show rainbow color
            uint32_t pixelColor = rgb.gamma32(rgb.ColorHSV(hue));
            rgb.setPixelColor(0, pixelColor);
            rgb.show();

            hue += 1000;  // step through hue
            if (hue >= 65535) {
                hue = 65535; // clamp at end
                rainbow_done = true;
            }
        } else {
            // Fade out
            fade_level = (fade_level > 5) ? fade_level - 5 : 0;
            rgb.setPixelColor(0, rgb.Color(0, 0, 0, fade_level)); // adjust brightness if your lib supports it
            rgb.show();
        }
    } else {
        // Static color
        rgb.setPixelColor(0, rgb.Color(color.red, color.green, color.blue));
        rgb.show();
    }
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
        
        case COMPLETED:
          color = COLOR_HOORAY;
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