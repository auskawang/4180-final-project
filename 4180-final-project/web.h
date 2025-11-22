#ifndef WEB_H
#define WEB_H
#include <WiFi.h>
void web_init();
void webserver_handle_client();
String webserver_read_request(WiFiClient& client);
void webserver_send_response(WiFiClient& client);
void webserver_send_page(WiFiClient &client);
void set_grams_left_and_tracker(float grams_left_param, float tracker_param);
void set_total_grams(float total_grams_param);

#endif