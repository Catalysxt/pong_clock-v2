#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <WiFi.h>
#include <time.h>
#include "config.h"
// ============================================
// TIME SYNCHRONIZATION MODULE
// ============================================
namespace TimeSync {
// Current time state
struct TimeState {
    int hour;
    int minute;
    int second;
    bool synced;
};
// Internal state
static TimeState currentTime = {12, 0, 0, false};
static unsigned long lastUpdate = 0;
// ============================================
// WiFi Event Callback (non-blocking approach)
// ============================================
void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("[TimeSync] WiFi connected");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("[TimeSync] IP: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[TimeSync] WiFi disconnected");
            break;
    }
}
// ============================================
// Initialize WiFi and NTP
// ============================================
bool init() {
    Serial.println("Connecting ...");
    
    // Register callback for WiFi events
    WiFi.onEvent(onWiFiEvent);
    
    // Start WiFi connection
    WiFi.begin(hw::network::kSsid, hw::network::kPassword);
    
    // Wait for connection (with timeout)
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[TimeSync] WiFi failed!");
        return false;
    }
    
    // Configure NTP
    configTime(
        hw::network::kGmtOffsetSec,
        hw::network::kDaylightOffsetSec,
        hw::network::kNtpServer
    );
    
    // Wait for time sync
    Serial.println("[TimeSync] Waiting for NTP...");
    struct tm timeinfo;
    int syncAttempts = 0;
    while (!getLocalTime(&timeinfo) && syncAttempts < 10) {
        delay(1000);
        Serial.print(".");
        syncAttempts++;
    }
    Serial.println();
    
    if (getLocalTime(&timeinfo)) {
        currentTime.synced = true;
        Serial.printf("[TimeSync] Synced: %02d:%02d:%02d\n",
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        return true;
    }
    
    Serial.println("[TimeSync] NTP failed!");
    return false;
}
// ============================================
// Update current time (call periodically)
// ============================================
void update() {
    // Only update every second
    if (millis() - lastUpdate < 1000) return;
    lastUpdate = millis();
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        currentTime.hour = timeinfo.tm_hour;
        currentTime.minute = timeinfo.tm_min;
        currentTime.second = timeinfo.tm_sec;
        currentTime.synced = true;
    }
}
// ============================================
// Getters
// ============================================
int getHour() {
    update();
    return currentTime.hour;
}
int getMinute() {
    update();
    return currentTime.minute;
}
int getSecond() {
    update();
    return currentTime.second;
}
bool isSynced() {
    return currentTime.synced;
}
// Get formatted time string "HH:MM"
void getFormattedTime(char* buffer, size_t len) {
    snprintf(buffer, len, "%02d:%02d", currentTime.hour, currentTime.minute);
}
}  // namespace TimeSync
#endif // TIME_SYNC_H