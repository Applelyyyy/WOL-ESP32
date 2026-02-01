#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>

const char* ssid     = "YOUR_WIFI_SSID";      // Change to your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";  // Change to your WiFi password

const char* serverName = "WOL-Server";        // Change to your server name
const char *MACAddress = "AA:BB:CC:DD:EE:FF"; // Change to your PC's MAC address
IPAddress pc_ip(0, 0, 0, 0);                  // Change to your PC's IP address Ex. IPAddress pc_ip(10, 10, 1, 11); 
const int serverPort = 44;                    // ESP32 web server port

WiFiUDP UDP;
WakeOnLan WOL(UDP);
WebServer server(serverPort);

bool isPcOn = false;

void checkPcStatus() {
  isPcOn = Ping.ping(pc_ip);
}

String getHTML() {
  String statusText = isPcOn ? "<span style='color:#2ecc71'>ONLINE</span>" : "<span style='color:#e74c3c'>OFFLINE</span>";
  String btnColor = isPcOn ? "#95a5a6" : "#3498db"; 
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'>";
  html += "<meta http-equiv='refresh' content='10'>";
  html += "<title>" + String(serverName) + " Control</title>";
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; background-color: #1a1a1a; color: white; padding: 20px; }";
  html += ".card { background: linear-gradient(145deg, #2a2a2a, #1f1f1f); width: 100%; max-width: 450px; padding: 30px 20px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.5); text-align: center; }";
  html += "h1 { font-size: clamp(1.5rem, 5vw, 2rem); margin-bottom: 10px; color: #fff; }";
  html += ".server-info { font-size: clamp(0.9rem, 3vw, 1rem); color: #888; margin-bottom: 20px; }";
  html += ".status { font-size: clamp(1.2rem, 4vw, 1.5rem); margin: 20px 0; padding: 15px; background: #222; border-radius: 10px; }";
  html += ".btn { display: block; width: 100%; border: none; color: white; padding: 16px; font-size: clamp(1rem, 3.5vw, 1.1rem); margin: 12px 0; cursor: pointer; border-radius: 12px; transition: all 0.3s; font-weight: 600; text-decoration: none; }";
  html += ".btn:active { transform: scale(0.95); }";
  html += ".btn-wake { background: linear-gradient(135deg, " + btnColor + ", " + btnColor + "cc); }";
  html += ".btn-wake:hover { box-shadow: 0 4px 15px rgba(52,152,219,0.4); }";
  html += ".refresh-note { font-size: clamp(0.75rem, 2.5vw, 0.85rem); color: #666; margin-top: 20px; }";
  html += "@media (max-width: 480px) { .card { padding: 25px 15px; } }";
  html += "</style></head><body>";
  
  html += "<div class='card'>";
  html += "<h1>" + String(serverName) + "</h1>";
  html += "<div class='server-info'>Server IP: " + pc_ip.toString() + ":" + String(serverPort) + "</div>";
  html += "<div class='status'>Status: " + statusText + "</div>";
  
  html += "<a href='/wake' class='btn btn-wake'>WAKE UP</a>";
  
  html += "<div class='refresh-note'>Auto-refresh every 10 seconds</div>";
  html += "</div></body></html>";
  return html;
}

void handleRoot() {
  checkPcStatus(); 
  server.send(200, "text/html", getHTML());
}

void handleWake() {
  Serial.println("\n[WAKE] Button pressed - Sending WOL packet...");
  Serial.print("[WAKE] Target MAC: ");
  Serial.println(MACAddress);
  Serial.print("[WAKE] Target IP: ");
  Serial.println(pc_ip);
  Serial.print("[WAKE] Broadcast Address: ");
  Serial.println(WiFi.broadcastIP());
  
  WOL.sendMagicPacket(MACAddress);
  
  Serial.println("[WAKE] Magic packet sent!");
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Booting Server...</title>";
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; background-color: #1a1a1a; color: white; }";
  html += ".container { text-align: center; }";
  html += "h1 { font-size: clamp(1.8rem, 5vw, 2.5rem); color: #2ecc71; margin-bottom: 20px; }";
  html += ".timer { font-size: clamp(3rem, 10vw, 5rem); font-weight: bold; color: #3498db; margin: 30px 0; }";
  html += ".message { font-size: clamp(1rem, 3vw, 1.2rem); color: #888; margin-top: 20px; }";
  html += ".progress-bar { width: 80%; max-width: 400px; height: 8px; background: #333; border-radius: 10px; margin: 30px auto; overflow: hidden; }";
  html += ".progress-fill { height: 100%; background: linear-gradient(90deg, #3498db, #2ecc71); transition: width 1s linear; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>[!] Signal Sent!</h1>";
  html += "<div class='message'>Wake-on-LAN packet delivered</div>";
  html += "<div class='timer' id='countdown'>60</div>";
  html += "<div class='message'>Waiting for system to boot...</div>";
  html += "<div class='progress-bar'><div class='progress-fill' id='progress'></div></div>";
  html += "</div>";
  html += "<script>";
  html += "let seconds = 60;";
  html += "const countdownEl = document.getElementById('countdown');";
  html += "const progressEl = document.getElementById('progress');";
  html += "progressEl.style.width = '100%';";
  html += "const interval = setInterval(() => {";
  html += "  seconds--;";
  html += "  countdownEl.textContent = seconds;";
  html += "  progressEl.style.width = (seconds / 60 * 100) + '%';";
  html += "  if (seconds <= 0) {";
  html += "    clearInterval(interval);";
  html += "    window.location.href = '/';";
  html += "  }";
  html += "}, 1000);";
  html += "</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  Serial.println("\n\n--- ESP32 Starting ---");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // After 60 attempts (30 seconds), restart ESP32 and try again
    if (attempts > 60) {
      Serial.println("\nWiFi connection timeout! Restarting ESP32...");
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connect at: http://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(serverPort);

  // Setup Wake-on-LAN
  WOL.setRepeat(3, 100); // Send packet 3 times with 100ms delay
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
  
  Serial.println("\n[WOL SETUP]");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Broadcast IP: ");
  Serial.println(WiFi.broadcastIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  // Auto-boot PC when ESP32 connects to WiFi
  Serial.println("\n[AUTO-BOOT] Sending Wake-on-LAN packet...");
  Serial.print("Target MAC: ");
  Serial.println(MACAddress);
  Serial.print("Target IP: ");
  Serial.println(pc_ip);
  
  WOL.sendMagicPacket(MACAddress);
  
  Serial.println("[AUTO-BOOT] Magic packet sent!");

  server.on("/", handleRoot);
  server.on("/wake", handleWake);
  server.begin();
  
  Serial.println("\nWeb server started!");
}

void loop() {
  server.handleClient();
}