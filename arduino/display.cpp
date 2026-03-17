#include "display.h"
#include "config.h"

#include <SPI.h>
#include <Adafruit_GFX.h>

// ============================================
// SPI BUS
// ============================================

SPIClass hspi(HSPI);

// ============================================
// DISPLAY INSTANCE
// ============================================

Adafruit_ST7789 tft(
    &hspi,
    hw::display::pins::kCS,
    hw::display::pins::kDC,
    hw::display::pins::kRST
);

// ============================================
// DISPLAY INIT
// ============================================

void displayInit() {

    pinMode(hw::display::pins::kBacklight, OUTPUT);
    digitalWrite(hw::display::pins::kBacklight, HIGH);

    hspi.begin(
        hw::display::pins::kSCK,
        hw::display::pins::kMISO,
        hw::display::pins::kMOSI,
        hw::display::pins::kCS
    );

    tft.init(240, 320);
    tft.setRotation(1);

    // Orientation fix for this panel
    uint8_t madctl =
        ST77XX_MADCTL_MX |
        ST77XX_MADCTL_MY |
        ST77XX_MADCTL_MV |
        ST77XX_MADCTL_RGB;

    tft.sendCommand(ST77XX_MADCTL, &madctl, 1);

    tft.fillScreen(activeTheme->background);
}

// ============================================
// DRAWING
// ============================================

void drawCentreLine() {

    const int x = hw::display::kWidth / 2;

    for (int y = hw::layout::kPlayAreaTop; y < hw::display::kHeight; y += 10) {

        tft.fillRect(
            x - 1,
            y,
            2,
            5,
            activeTheme->centreLine
        );
    }
}

void drawBall(int x, int y) {

    tft.fillCircle(
        x,
        y,
        BALL_R,
        activeTheme->ball
    );
}

void eraseBall(int x, int y) {

    tft.fillCircle(
        x,
        y,
        BALL_R,
        activeTheme->background
    );
}

void drawPaddle(int x, int y) {

    tft.fillRect(
        x,
        y,
        PADDLE_W,
        PADDLE_H,
        activeTheme->paddle
    );
}

void erasePaddle(int x, int y) {

    tft.fillRect(
        x,
        y,
        PADDLE_W,
        PADDLE_H,
        activeTheme->background
    );
}

void drawScore(int leftScore, int rightScore) {

    tft.setTextSize(3);

    tft.setTextColor(
        activeTheme->score,
        activeTheme->background
    );

    int y = hw::layout::kScoreY;

    // Format scores with leading zeros

    char buffer[6];
    sprintf(buffer, "%02d:%02d", leftScore, rightScore);

    // Center text

    int x = (hw::display::kWidth - 6 * 6 * 3) / 2;

    tft.setCursor(x, y);

    tft.print(buffer);
}
