#include <Arduino.h>

#define RED_PIN    15   // Replace with your actual GPIO
#define GREEN_PIN  23
#define BLUE_PIN   22
#define SPEAKER_PIN 21
#define PWM_FREQ   5000
#define PWM_RES    8    // 8-bit resolution (0-255)

// Fade duration
#define FADE_MS 2000

bool red = false;

void taskFadeRGB(void *pv) {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  while (1) {
    red = false;
    // Fade green -> red
    for (int i = 0; i <= 255; i++) {
      analogWrite(RED_PIN, i);        // Red increases
      analogWrite(GREEN_PIN, 255 - i); // Green decreases
      analogWrite(BLUE_PIN, 0);        // Blue off
      vTaskDelay(pdMS_TO_TICKS(FADE_MS / 255));
    }
    red = true;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskAudibleAlert(void *pv) {
  pinMode(SPEAKER_PIN, OUTPUT);

  while (1) {
    if (red) {
      tone(SPEAKER_PIN, 1000);
    }
    else {
      noTone(SPEAKER_PIN);
    }
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreate(taskFadeRGB, "taskFadeRGB", 2048, NULL, 1, NULL);
  xTaskCreate(taskAudibleAlert, "taskAudibleAlert", 2048, NULL, 1, NULL);
}

void loop() {
  
}