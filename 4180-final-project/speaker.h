#ifndef SPEAKER_H
#define SPEAKER_H

#include <Arduino.h>

// Configuration constants
#define SPEAKER_PIN 21
#define ALERT_FREQUENCY 1000  // Hz
#define ALERT_DURATION 500    // ms
#define ALERT_INTERVAL 30000  // 30 seconds between alerts

// Speaker states
enum SpeakerState {
  SPEAKER_IDLE,
  SPEAKER_ALERTING
};

// Function prototypes
void speaker_init();
void speaker_alert();
void speaker_stop();
bool speaker_should_alert();
void speaker_task_handler();

#endif // SPEAKER_H