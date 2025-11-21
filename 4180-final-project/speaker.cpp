#include "speaker.h"

static SpeakerState current_state = SPEAKER_IDLE;
static unsigned long last_alert_time = 0;
static bool alert_enabled = false;

void speaker_init() {
  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);
  current_state = SPEAKER_IDLE;
  last_alert_time = 0;
  Serial.println("Speaker initialized");
}

void speaker_alert() {
  if (current_state == SPEAKER_IDLE) {
    current_state = SPEAKER_ALERTING;
    
    // Generate tone using PWM - ESP32-C6 compatible
    ledcAttach(SPEAKER_PIN, ALERT_FREQUENCY, 8);  // pin, frequency, 8-bit resolution
    ledcWrite(SPEAKER_PIN, 128);  // 50% duty cycle
    
    delay(ALERT_DURATION);
    
    // Stop tone
    ledcWrite(SPEAKER_PIN, 0);
    ledcDetach(SPEAKER_PIN);
    
    current_state = SPEAKER_IDLE;
    last_alert_time = millis();
    
    Serial.println("Alert played");
  }
}

void speaker_stop() {
  ledcWrite(SPEAKER_PIN, 0);
  ledcDetach(SPEAKER_PIN);
  digitalWrite(SPEAKER_PIN, LOW);
  current_state = SPEAKER_IDLE;
}

bool speaker_should_alert() {
  unsigned long current_time = millis();
  return (current_time - last_alert_time >= ALERT_INTERVAL);
}

void speaker_task_handler() {
  // This function will be called repeatedly by taskAlertUser
  if (alert_enabled && speaker_should_alert()) {
    speaker_alert();
  }
}