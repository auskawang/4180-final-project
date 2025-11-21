#include "status_led.h"

static LEDColor current_color = COLOR_OFF;

// Helper function to invert PWM value for common anode LEDs
static uint8_t adjust_pwm(uint8_t value) {
  if (COMMON_ANODE) {
    return 255 - value;
  }
  return value;
}

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
  
  Serial.println("RGB LED initialized");
  Serial.printf("LED Pins - R:%d G:%d B:%d\n", LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN);
  Serial.printf("LED Type: %s\n", COMMON_ANODE ? "Common Anode" : "Common Cathode");
}

void status_led_set_color(LEDColor color) {
  current_color = color;
  
  // Write PWM values to each channel (adjusted for LED type)
  ledcWrite(LED_RED_PIN, adjust_pwm(color.red));
  ledcWrite(LED_GREEN_PIN, adjust_pwm(color.green));
  ledcWrite(LED_BLUE_PIN, adjust_pwm(color.blue));
  
  Serial.printf("LED Color: R=%d G=%d B=%d\n", color.red, color.green, color.blue);
}

void status_led_set_from_hydration_state(HydrationState state) {
  LEDColor color;
  
  switch(state) {
    case HYDRATION_GOOD:
      color = COLOR_GOOD;
      Serial.println("LED: ✅ GREEN (Good hydration)");
      break;
      
    case HYDRATION_OK:
      color = COLOR_OK;
      Serial.println("LED: 💛 YELLOW (OK hydration)");
      break;
      
    case HYDRATION_LOW:
      color = COLOR_LOW;
      Serial.println("LED: 🟠 ORANGE (Low hydration)");
      break;
      
    case HYDRATION_CRITICAL:
      color = COLOR_CRITICAL;
      Serial.println("LED: 🔴 RED (Critical - drink water!)");
      break;
      
    default:
      color = COLOR_OFF;
      break;
  }
  
  status_led_set_color(color);
}

void status_led_update() {
  // Get current hydration state and update LED accordingly
  HydrationState state = hydration_get_state();
  status_led_set_from_hydration_state(state);
}

void status_led_off() {
  LEDColor off = COLOR_OFF;
  status_led_set_color(off);
}

void status_led_test_sequence() {
  Serial.println("\n🌈 Testing RGB LED colors...");
  
  // Red
  Serial.println("Red...");
  LEDColor red = COLOR_CRITICAL;
  status_led_set_color(red);
  delay(1000);
  
  // Orange
  Serial.println("Orange...");
  LEDColor orange = COLOR_LOW;
  status_led_set_color(orange);
  delay(1000);
  
  // Yellow
  Serial.println("Yellow...");
  LEDColor yellow = COLOR_OK;
  status_led_set_color(yellow);
  delay(1000);
  
  // Green
  Serial.println("Green...");
  LEDColor green = COLOR_GOOD;
  status_led_set_color(green);
  delay(1000);
  
  // Blue (bonus test color)
  Serial.println("Blue...");
  LEDColor blue = {0, 0, 255};
  status_led_set_color(blue);
  delay(1000);
  
  // White (all colors)
  Serial.println("White...");
  LEDColor white = {255, 255, 255};
  status_led_set_color(white);
  delay(1000);
  
  // Off
  Serial.println("Off...");
  status_led_off();
  delay(500);
  
  Serial.println("RGB LED test complete!\n");
}
```

## Wiring Guide for RGB LED:

**Common Cathode RGB LED (most common):**
```
ESP32-C6          RGB LED
GPIO 6    ─────>  R (Red pin) ──┬── 220Ω resistor
GPIO 7    ─────>  G (Green pin) ┼── 220Ω resistor
GPIO 10   ─────>  B (Blue pin) ─┴── 220Ω resistor
                  - (Cathode)  ───── GND
```

**Common Anode RGB LED:**
```
ESP32-C6          RGB LED
GPIO 6    ─────>  R (Red pin) ──┬── 220Ω resistor
GPIO 7    ─────>  G (Green pin) ┼── 220Ω resistor
GPIO 10   ─────>  B (Blue pin) ─┴── 220Ω resistor
                  + (Anode)    ───── 3.3V