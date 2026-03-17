#pragma once

#include <Adafruit_ST7789.h>

// Global display instance
extern Adafruit_ST7789 tft;

// Initialization
void displayInit();

// Drawing helpers
void drawCentreLine();

void drawBall(int x, int y);
void eraseBall(int x, int y);

void drawPaddle(int x, int y);
void erasePaddle(int x, int y);

void drawScore(int leftScore, int rightScore);
