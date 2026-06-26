#include <WiFi.h>
#include <WebServer.h>

// --- Config ---
const char* ssid = "HI";
const char* password = "HIHIHIHI";

WebServer server(80);

// --- Global States ---
float temp = 30, current = 0;
bool vib = false;
bool sensorsHalted = false;
String systemStatus = "Monitoring Active";

// --- HTML Dashboard ---
// (We use a simple built-in HTML here, but see the 'docs' folder for the premium web UI simulation!)
String getHTML() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f4f4f4;}";
  html += ".card{background:white; padding:20px; margin:10px; border-radius:10px; display:inline-block; width:160px; box-shadow: 0 4px 8px rgba(0,0,0,0.1);}";
  html += ".halt{background:#e74c3c; color:white; padding:15px; border:none; border-radius:5px; cursor:pointer;}";
  html += ".resume{background:#2ecc71; color:white; padding:15px; border:none; border-radius:5px; cursor:pointer;}</style></head><body>";
  html += "<h1>Machine Health Hub</h1>";
  
  if (sensorsHalted) {
    html += "<h2 style='color:#e67e22;'>⚠ MANUAL QC MODE: SENSORS PAUSED</h2>";
    html += "<a href='/resume'><button class='resume'>RESUME SYSTEM</button></a>";
  } else {
    html += "<div class='card'><h3>Temp</h3><h2 style='color:#c0392b;'>" + String(temp, 1) + " C</h2></div>";
    html += "<div class='card'><h3>Current</h3><h2 style='color:#2980b9;'>" + String(current, 2) + " A</h2></div>";
    html += "<div class='card'><h3>Vib</h3><h2>" + String(vib ? "HIGH" : "LOW") + "</h2></div>";
    html += "<p>Status: <b>" + systemStatus + "</b></p>";
    html += "<br><a href='/halt'><button class='halt'>HALT FOR MAINTENANCE</button></a>";
  }
  html += "</body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", getHTML()); }
void handleHalt() { sensorsHalted = true; systemStatus = "Paused for QC"; server.sendHeader("Location", "/"); server.send(303); }
void handleResume() { sensorsHalted = false; systemStatus = "Resumed"; server.sendHeader("Location", "/"); server.send(303); }

void setup() {
  Serial.begin(115200);
  pinMode(18, INPUT); // SW-420 on Pin 18
  pinMode(2, OUTPUT);  // Status LED
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  server.on("/", handleRoot);
  server.on("/halt", handleHalt);
  server.on("/resume", handleResume);
  server.begin();
  
  Serial.println("\nReady. Access IP: " + WiFi.localIP().toString());
}

void loop() {
  server.handleClient();
  
  if (!sensorsHalted) {
    // 1. SENSOR LOGIC
    // Temperature conversion (LM35)
    temp = (analogRead(34) * 3300.0 / 4095.0) / 10.0;
    
    // Current conversion (ACS712 - 5A version)
    // Offset 2048 for 3.3V system at 1.65V center
    int rawValue = analogRead(35);
    current = abs(rawValue - 2048) * (3.3 / 4095.0) / 0.185; 
    
    // Vibration (SW-420)
    vib = digitalRead(18);

    // 2. LED FEEDBACK (Blink if Over Temp)
    if (temp > 30.0) digitalWrite(2, HIGH); 
    else digitalWrite(2, LOW);
  }
}
