#pragma once
#include <Arduino.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


// ============================================
// HARDWARE CONFIGURATION
// ============================================

namespace hw {

namespace display {

constexpr uint16_t kWidth  = 320;
constexpr uint16_t kHeight = 240;

namespace pins {
constexpr int8_t kCS        = 15;
constexpr int8_t kDC        = 2;
constexpr int8_t kRST       = -1;
constexpr int8_t kMOSI      = 13;
constexpr int8_t kMISO      = 12;
constexpr int8_t kSCK       = 14;
constexpr int8_t kBacklight = 27;
}

}  // namespace display

namespace layout {
constexpr int kScoreY       = 10; // Score Y position
constexpr int kScoreHeight  = 32; // Text height at size 4
constexpr int kPlayAreaTop  = 50; // Ball/paddle ceiling
constexpr int kPaddleMargin = 10; // Distance from screen edge
}

}  // namespace hw

// ============================================
// GAME CONSTANTS
// ============================================

constexpr int PADDLE_W = 10;
constexpr int PADDLE_H = 50;
constexpr int BALL_R   = 4;

constexpr int LEFT_X   = hw::layout::kPaddleMargin;
constexpr int RIGHT_X  = hw::display::kWidth - hw::layout::kPaddleMargin - PADDLE_W;

// ============================================
// THEME SYSTEM
// ============================================

struct Theme {
  uint16_t background;
  uint16_t ball;
  uint16_t paddle;
  uint16_t score;
  uint16_t centreLine;
  uint16_t particle;
};

namespace themes {

constexpr Theme kCyberpunk = {
  0x0000, // background
  0xF81F, // ball
  0x07FF, // paddle
  0xFFFF, // score
  0x4208, // centre line
  0xF81F  // particle
};

constexpr Theme kGlacier = {
  0x001F,
  0x07FF,
  0xFFFF,
  0xFFFF,
  0x4208,
  0x07FF
};

constexpr Theme kMonochrome = {
  ST77XX_BLUE,
  ST77XX_RED,
  ST77XX_MAGENTA,
  ST77XX_GREEN,
  0x4208,
  0xFFFF
};

constexpr Theme kMatrix = {
  0x0000,
  0x07E0,
  0x07E0,
  0x07E0,
  0x4208,
  0x07E0
};

}  // namespace themes

inline const Theme* activeTheme = &themes::kMonochrome;
