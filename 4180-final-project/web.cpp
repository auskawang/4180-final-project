#include <WiFi.h>
#include "web.h"
#include "storage.h"
float web_goal_grams;
float web_total_grams;
Entry web_entries[MAX_ENTRIES];
//--------------------------------------------------
// WiFi Settings
//--------------------------------------------------
const char* ssid     = "ESP32-14";
const char* password = "12345678";

WiFiServer server(80);

//--------------------------------------------------
// Initialize WiFi SoftAP
//--------------------------------------------------
void web_init() {
  Serial.println("Starting WiFi AP...");
  WiFi.softAP(ssid, password);

  Serial.print("AP Running. IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
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
void webserver_handle_client() {
  WiFiClient client = server.available();
  if (!client) return;

  // Read request
  String request = webserver_read_request(client);

  //--------------------------------------------------
  // AJAX endpoint
  //--------------------------------------------------
  if (request.indexOf("GET /data") >= 0) {
    webserver_handle_data(client);
    client.stop();
    return;
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
      return;
  }
  //--------------------------------------------------
  // Main HTML page
  //--------------------------------------------------
  webserver_send_page(client);
  client.stop();

  Serial.println("Client disconnected.");
}
