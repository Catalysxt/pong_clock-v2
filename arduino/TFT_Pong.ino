#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "game.h"
#include "time_sync.h"

void setup() {
    Serial.begin(115200);

    displayInit();

    tft.setCursor(80, 110);
    tft.setTextSize(4);
    tft.setTextColor(activeTheme->score, activeTheme->background);
    tft.print("Connecting...");

    syncTime();

    int hour, minute;
    getCurrentTime(hour, minute);

    scoreLeft = hour;
    scoreRight = minute;

    tft.fillScreen(activeTheme->background);

    gameInit();
}

void loop() {
    updateTargetTime();

    scoreLeft = getTargetHour();
    scoreRight = getTargetMinute();
    drawScore(scoreLeft, scoreRight);

    gameUpdate();

    delay(16);
}
