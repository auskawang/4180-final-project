#include <WiFi.h>
#include "web.h"
#include "storage.h"
#include "state.h"
float web_goal_grams;
float web_total_grams;
Entry web_entries[MAX_ENTRIES];
bool web_request = false;

float user_goal_grams = 2000;  // default daily goal
float user_duration_hours = 16; // default period
//--------------------------------------------------
// WiFi Settings
//--------------------------------------------------
const char* ssid     = "Hydration Tracker";
const char* password = "12345678";

WiFiServer server(80);
unsigned long last_isr = 0;
void set_user_goal(float goal) {
    user_goal_grams = goal;
}

void set_user_duration(float duration) {
    user_duration_hours = duration;
}

float get_user_goal() {
    return user_goal_grams;
}

float get_user_duration() {
    return user_duration_hours;
}
void IRAM_ATTR buttonISR() {
    unsigned long now = millis();
    if (now - last_isr > 50) {  // 50ms debounce
        web_request = true;
    }
    last_isr = now;
}

bool get_web_request() {
  return web_request;
}

void set_web_pin_state(uint8_t state) {
  digitalWrite(WEB_STATUS_PIN, state);
}

void set_web_request(bool input) {
  web_request = input;
}
//--------------------------------------------------
// Initialize WiFi SoftAP
//--------------------------------------------------
void web_init() {
  pinMode(WEB_STATUS_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(
        digitalPinToInterrupt(BTN_PIN),         
        buttonISR,
        FALLING             
  );
}

void web_enable() {
  if (DEBUG)
    Serial.println("Starting WiFi AP...");
  WiFi.softAP(ssid, password);

  if (DEBUG)
    Serial.print("AP Running. IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  set_web_pin_state(HIGH);
}

void web_disable() {
  if (DEBUG) Serial.println("Stopping server and turning off WiFi.");
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  set_web_pin_state(LOW);
}

void set_goal_grams(float goal_grams_param) {
  web_goal_grams = goal_grams_param;
}

void set_total_grams(float total_grams_param) {
  web_total_grams = total_grams_param;
}

void set_history(Entry entries[]) {
  for (int i = 0; i < MAX_ENTRIES; i++) {
      web_entries[i] = entries[i];
  }
}

//--------------------------------------------------
// Read HTTP Request Header
//--------------------------------------------------
String webserver_read_request(WiFiClient &client) {
  String request = "";
  unsigned long timeout = millis();

  while (client.connected() && millis() - timeout < 2000) {
    if (client.available()) {
      char c = client.read();
      request += c;

      // End of headers (blank line)
      if (request.endsWith("\r\n\r\n")) {
        break;
      }
    }
  }
  return request;
}

//--------------------------------------------------
// Handle AJAX data endpoint  (/data)
//--------------------------------------------------
void webserver_handle_data(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  client.print("{\"web_goal_grams\":");
  client.print(web_goal_grams, 1);
  client.print(",\"web_total_grams\":");
  client.print(web_total_grams, 1);

  // ---- NEW: Send history array ----
  client.print(",\"history\":[");
  for (int i = 0; i < MAX_ENTRIES; i++) {
    client.print("{\"d\":");
    client.print(web_entries[i].grams_drank);
    client.print(",\"g\":");
    client.print(web_entries[i].goal);
    client.print("}");
    if (i < MAX_ENTRIES - 1) client.print(",");
  }
  client.print("]}");
}

//--------------------------------------------------
// Serve HTML Page
//--------------------------------------------------
void webserver_send_page(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html><html>");
  client.println("<head><title>Hydration Monitor</title></head>");
  client.println("<body style='font-family:sans-serif;'>");
  client.println("<div style='text-align: center;'>");
  client.println("<h1>Hydration Monitor</h1>");

  client.println("<p>Your Goal Today is: <span id='web_goal_grams'>--</span>mL</p>");
  client.println("<p>You Have Drank <span id='web_total_grams'><b>--</b></span> mL Today</p>");
  client.println("<div>");

  // if (get_state() == STATE_WAITING_USER_INPUT) {
  //   //----------------------------------------------------
  //   // OVERLAY FOR USER INPUT
  //   //----------------------------------------------------
  //   client.println(R"rawliteral(
  //   <style>
  //     /* Overlay styling */
  //     #overlay {
  //       position: fixed;
  //       top: 0; left: 0;
  //       width: 100%; height: 100%;
  //       background: rgba(0,0,0,0.7);
  //       display: flex;
  //       align-items: center;
  //       justify-content: center;
  //       z-index: 1000;
  //     }
  //     #overlay-box {
  //       background: white;
  //       padding: 20px;
  //       border-radius: 10px;
  //       text-align: center;
  //       min-width: 250px;
  //     }
  //     #overlay-box input {
  //       width: 80px;
  //       margin: 5px;
  //     }
  //     #overlay-box button {
  //       margin-top: 10px;
  //       padding: 5px 15px;
  //     }
  //   </style>

  //   <div id="overlay">
  //     <div id="overlay-box">
  //       <h2>Set Goal</h2>
  //       <div>
  //         <label>Duration (hours): </label>
  //         <input type="number" id="inputDuration" min="1" max="24">
  //       </div>
  //       <div>
  //         <label>Goal (mL): </label>
  //         <input type="number" id="inputGoal" min="100">
  //       </div>
  //       <button onclick="submitOverlay()">Enter</button>
  //     </div>
  //   </div>

  //   <script>
  //     function submitOverlay() {
  //       // read input values
  //       const duration = document.getElementById('inputDuration').value;
  //       const goal = document.getElementById('inputGoal').value;

  //       // hide the overlay
  //       document.getElementById('overlay').style.display = 'none';

  //       // call a function to handle these values
  //       setGoalAndDuration(duration, goal);
  //     }

  //     function setGoalAndDuration(duration, goal) {
  //       // Example: Send values to ESP32 via AJAX
  //       fetch(`/set_goal?duration=${duration}&goal=${goal}`)
  //         .then(response => response.text())
  //         .then(data => console.log("Goal updated:", data));
  //     }
  //   </script>
  //   )rawliteral");
  // }

  //----------------------------------------------------
  // TABLE OF HISTORY ENTRIES
  //----------------------------------------------------
  client.println("<h3>History (Last 7 days)</h3>");
  client.println("<table style='margin-left:auto; margin-right:auto; border-collapse:collapse; border:1px solid black; width:60%;'>");
  client.println("<tr><th style='border:1px solid black; padding:8px;'>Index</th><th style='border:1px solid black; padding:8px;'>Drank (g)</th><th style='border:1px solid black; padding:8px;'>Goal (g)</th></tr>");
  client.println("<tbody id='historyTable'></tbody>");
  client.println("</table>");
  client.println("<br>");
  client.println("<button onclick=\"fetch('/action')\" style='padding:10px 20px; font-size:14px;'>Reset History</button>");

  //----------------------------------------------------
  // AJAX SCRIPT
  //----------------------------------------------------
  client.println("<script>");
  client.println("setInterval(function(){");
  client.println("  fetch('/data')");
  client.println("    .then(r => r.json())");
  client.println("    .then(d => {");
  client.println("       document.getElementById('web_goal_grams').innerHTML = d.web_goal_grams;");
  client.println("       document.getElementById('web_total_grams').innerHTML = '<b>' + d.web_total_grams + '</b>';");
  client.println("       let tableHTML = '';");
  client.println("       for (let i = 0; i < d.history.length; i++) {");
  client.println("         tableHTML += '<tr>' +");
  client.println("           '<td>' + i + '</td>' +");
  client.println("           '<td>' + d.history[i].d + '</td>' +");
  client.println("           '<td>' + d.history[i].g + '</td>' +");
  client.println("         '</tr>'; ");
  client.println("       }");
  client.println("       document.getElementById('historyTable').innerHTML = tableHTML;");
  client.println("    });");
  client.println("}, 1000);");
  client.println("</script>");

  client.println("</body></html>");
}

//--------------------------------------------------
// Main webserver handler (call inside a task)
//--------------------------------------------------
bool webserver_handle_client() {
  WiFiClient client = server.available();
  if (!client) return false;

  // Read request
  String request = webserver_read_request(client);

  //--------------------------------------------------
  // AJAX endpoint
  //--------------------------------------------------
  if (request.indexOf("GET /data") >= 0) {
    webserver_handle_data(client);
    client.stop();
    return true;
  }

  //reset button
  if (request.indexOf("GET /action") >= 0) {
      storage_reset_entries();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Action completed.");
      client.stop();
      return true;
  }

  // Handle goal/duration update
  if (request.indexOf("GET /set_goal") >= 0) {
    // Parse query parameters
    int idxGoal = request.indexOf("goal=");
    int idxDuration = request.indexOf("duration=");
    if (idxGoal >= 0 && idxDuration >= 0) {
      float goal = request.substring(idxGoal + 5, request.indexOf('&', idxGoal)).toFloat();
      float duration = request.substring(idxDuration + 9, request.indexOf(' ', idxDuration)).toFloat();

      set_user_goal(goal);
      set_user_duration(duration);
      change_to_running();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Goal and duration updated.");
      client.stop();
      return true;
    }
  }
  //--------------------------------------------------
  // Main HTML page
  //--------------------------------------------------
  webserver_send_page(client);
  client.stop();

  Serial.println("Client disconnected.");
}
