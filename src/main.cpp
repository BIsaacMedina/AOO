#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <BasicOTA.hpp>

#define LED 2
#define outPin 13 // Outputs power
#define inPin 23  // Tests output from motherboard power light to detect ON/OFF

BasicOTA OTA;

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Globals
String pcState = "UNKNOWN";
int shutdownHour = 22;
int shutdownMinute = 1;
int turnOnHour = 6;
int turnOnMinute = 0;

WebServer server(80);

void setPCState(bool isOn) {
  pcState = isOn ? "ON" : "OFF";
  Serial.println("PC State set to: " + pcState);
}

String getPCState() {
  return pcState;
}

void switchOn() {
  if (digitalRead(inPin) == LOW) {
    digitalWrite(outPin, LOW);
    delay(1000);
    digitalWrite(outPin, HIGH);
    Serial.println("Switched PC ON.");
    setPCState(true);
  } else {
    Serial.println("PC ALREADY ON");
    setPCState(true);
  }
}

void switchOff() {
  if (digitalRead(inPin) == LOW) {
    digitalWrite(outPin, LOW);
    delay(5000);
    digitalWrite(outPin, HIGH);
    Serial.println("Switched PC OFF.");
    setPCState(false);
  } else {
    Serial.println("PC ALREADY OFF");
    setPCState(false);
  }
}

void handleStatus() {
  server.send(200, "application/json", "{\"state\": \"" + getPCState() + "\"}");
}

void handleSetShutdown() {
  if (server.hasArg("hour") && server.hasArg("minute")) {
    shutdownHour = server.arg("hour").toInt();
    shutdownMinute = server.arg("minute").toInt();
    server.send(200, "text/plain", "Shutdown time updated");
    Serial.printf("New shutdown time set: %02d:%02d\n", shutdownHour, shutdownMinute);
  } else {
    server.send(400, "text/plain", "Missing parameters: hour and minute");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(outPin, OUTPUT);
  pinMode(inPin, INPUT);
  digitalWrite(outPin, HIGH); // Default HIGH

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected.");

  OTA.begin();

  // Configure system time using NTP with dynamic timezone support
  setenv("TZ", "UTC", 1);   // Change "UTC" to your timezone string, e.g., "PST8PDT", "EST5EDT", etc.
  tzset();
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Waiting for NTP time...");
    delay(1000);
  }
  Serial.println("Time synced.");

  // Initial state detection
  setPCState(digitalRead(inPin) == HIGH);

  // Web routes
  server.on("/api/status", handleStatus);
  server.on("/api/set_shutdown", handleSetShutdown);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(500);

  OTA.handle();
  server.handleClient();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    Serial.print("Current time: ");
    Serial.println(timeStr);

    if (timeinfo.tm_hour == turnOnHour && timeinfo.tm_min == turnOnMinute && timeinfo.tm_sec == 0) {
      switchOn();
    }

    if (timeinfo.tm_hour == shutdownHour && timeinfo.tm_min == shutdownMinute && timeinfo.tm_sec == 0) {
      switchOff();
    }
  } else {
    Serial.println("Failed to get time.");
  }

  digitalWrite(LED, LOW);
  delay(500);
}
