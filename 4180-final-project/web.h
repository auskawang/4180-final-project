#ifndef WEB_H
#define WEB_H
#include <WiFi.h>
#include "config.h"

void web_init();
bool webserver_handle_client();
String webserver_read_request(WiFiClient& client);
void webserver_send_response(WiFiClient& client);
void webserver_send_page(WiFiClient &client);
void set_goal_grams(float goal_grams_param);
void set_total_grams(float total_grams_param);
void set_history(Entry entries[]);
bool get_web_request();
void set_web_request(bool input);
void web_enable();
void web_disable();
float get_user_goal();
float get_user_duration();
#endif