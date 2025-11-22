#include <WiFi.h>

float web_grams_left;
float web_tracker;
float web_total_grams;

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

void set_grams_left_and_tracker(float grams_left_param, float tracker_param) {
  web_grams_left = grams_left_param;
  web_tracker = tracker_param; 
}

void set_total_grams(float total_grams_param) {
  web_total_grams = total_grams_param;
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

  client.print("{\"web_grams_left\":");
  client.print(web_grams_left, 1);
  client.print(",\"web_tracker\":");
  client.print(web_tracker, 1);
  client.print(",\"web_total_grams\":");
  client.print(web_total_grams, 1);
  client.println("}");
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
  client.println("<h2>Hydration Monitor</h2>");

  client.println("<p>web_grams_left: <span id='web_grams_left'>--</span> ml</p>");
  client.println("<p>Tracker: <span id='web_tracker'>--</span></p>");
  client.println("<p>Total Hydration: <span id='web_total_grams'>--</span> ml</p>");

  // Javascript AJAX polling
  client.println("<script>");
  client.println("setInterval(function(){");
  client.println("  fetch('/data')");
  client.println("    .then(r => r.json())");
  client.println("    .then(d => {");
  client.println("       document.getElementById('web_grams_left').innerHTML = d.web_grams_left;");
  client.println("       document.getElementById('web_tracker').innerHTML = d.web_tracker;");
  client.println("       document.getElementById('web_total_grams').innerHTML = d.web_total_grams;");
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

  //--------------------------------------------------
  // Main HTML page
  //--------------------------------------------------
  webserver_send_page(client);
  client.stop();

  Serial.println("Client disconnected.");
}
