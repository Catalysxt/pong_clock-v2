#include <WiFi.h>
#include <time.h>
#include "wifi_config.h"
#include "time_sync.h"

static int targetHour = 0;
static int targetMinute = 0;

static unsigned long lastTimeCheck = 0;

void syncTime() {

  Serial.println("Connecting to WiFi...");

  WiFi.begin(wifi_config::kSsid, wifi_config::kPassword);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi failed!");
    return;
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  configTime(
       wifi_config::kGmtOffsetSec,
       wifi_config::kDaylightOffsetSec,
       wifi_config::kNtpServer);

  struct tm timeinfo;

  int syncAttempts = 0;

  while (!getLocalTime(&timeinfo) && syncAttempts < 10) {
    delay(1000);
    Serial.print(".");
    syncAttempts++;
  }

  if (getLocalTime(&timeinfo)) {

    targetHour = timeinfo.tm_hour;
    targetMinute = timeinfo.tm_min;

    Serial.printf(
      "\nTime synced: %02d:%02d\n",
      targetHour,
      targetMinute
    );

  } else {
    Serial.println("\nNTP sync failed!");
  }
}

void getCurrentTime(int& hour, int& minute) {

  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {

    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;

  } else {

    hour = 12;
    minute = 0;

  }
}

void updateTargetTime() {

  if (millis() - lastTimeCheck < 10000)
    return;

  lastTimeCheck = millis();

  int newHour, newMinute;

  getCurrentTime(newHour, newMinute);

  if (newMinute != targetMinute) {

    targetHour = newHour;
    targetMinute = newMinute;

    Serial.printf(
      "Time changed: %02d:%02d\n",
      targetHour,
      targetMinute
    );
  }
}

int getTargetHour() {
    return targetHour;
}

int getTargetMinute() {
    return targetMinute;
}
