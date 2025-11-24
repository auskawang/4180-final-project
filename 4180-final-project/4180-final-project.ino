#include <Arduino.h>
#include "speaker.h"
#include "hydration.h"
#include "status_led.h"
#include "scale.h"
#include "storage.h"
#include "web.h"
#include "state.h"

/*
Status LED that ranges from green (on track on hydration) to red (need to drink water to stay on track)
*/
void taskUpdateStatusLED(void *pv) {
  while(1) {
    //until user enters input for the session, make LED turn white
    while (get_state() == STATE_WAITING_USER_INPUT) {
      status_led_show_waiting();
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    status_led_update(get_hydration_state());
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
Speaker that audibly alerts the user when the LED is red and the user needs to drink water
*/
void taskAlertUser(void *pv) {
  while(1) {
    //while waiting for user input don't use the speaker
    while (get_state() == STATE_WAITING_USER_INPUT) {
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (get_hydration_state() == CRITICAL) {
      speaker_alert();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/*
Generates HTML page allowing user to view hydration data and interact with system wirelessly through Wifi
*/
void taskHTMLPage(void *pv) {
  while(1) {   
    //if user presses web page button
    if (get_web_request()) { 
      //turn on web page
      web_enable();
      unsigned long client_connect_time = millis();
      
      //keep web page on until 60 seconds pass with no client
      while(1) {
        bool no_client = false;

        if (get_state() == STATE_RUNNING) {
          set_goal_grams(read_goal_grams());
          set_total_grams(read_total_grams());
        }
        
        //if true, then there is a client connected
        if (webserver_handle_client()) {
          client_connect_time = millis();
        }
        if (millis() - client_connect_time > CLIENT_TIMEOUT_SECS * 1000) {
          break;
        }
        vTaskDelay(5);
      }
      set_web_request(false);
      web_disable();
    }
    vTaskDelay(10);
  }
}

/*
Detects scale changes and stores meaningful changes in non-volatile memory
*/
void taskReadScale(void *pv) {
  float prev_weight = 0;
  Entry html_page_entries[MAX_ENTRIES];

  while (1) {
    while (get_state() == STATE_WAITING_USER_INPUT) {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    float current_weight = scale_read_weight();
    
    // If there is something on the scale, weight decreased, and difference is meaningful
    if (current_weight > 30.0 && current_weight < prev_weight && prev_weight - current_weight > 30.0) {
      record_grams_drank(prev_weight - current_weight);  // This updates today's consumption
    }
    
    if (current_weight > 30.0)
      prev_weight = current_weight;
      
    update_hydration_status();

    //if the tracker is 0, then that means the "day" ended, so call reset function to reset hydration values and store grams of water drank for that day
    if (read_tracker() == 0) {
      storage_add_entry(read_total_grams(), read_goal_grams(), read_time_length() * 1000);
      storage_load_entries(html_page_entries);
      set_history(html_page_entries);
      //reset();
      current_state = STATE_WAITING_USER_INPUT;
      if (DEBUG) Serial.println(current_state);

      //storage_print_entries();
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  scale_init();
  status_led_init();
  web_init();
  speaker_init();
  storage_init();
  xTaskCreate(taskUpdateStatusLED, "taskUpdateStatusLED", 2048, NULL, 3, NULL);
  xTaskCreate(taskAlertUser, "taskAlertUser", 2048, NULL, 3, NULL);
  xTaskCreate(taskReadScale, "taskReadScale", 4096, NULL, 1, NULL);
  xTaskCreate(taskHTMLPage, "taskHTMLPage", 4096, NULL, 2, NULL);
}

void loop() {

}