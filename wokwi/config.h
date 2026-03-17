/*
  Adjust the configuration of the game including hardware connections and colors of
  UI elements
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <Adafruit_ILI9341.h>

// ============================================
// HARDWARE CONFIGURATION
// ============================================
namespace hw {

namespace display {
    constexpr uint16_t kWidth = 320;
    constexpr uint16_t kHeight = 240;
    
    namespace pins {
        constexpr int8_t kCS  = 15;
        constexpr int8_t kDC  = 2;
        constexpr int8_t kRST = 4;
        // MOSI (23), SCK (18), MISO (19) handled by SPI library
    }	
}

// A custom region to house the clock. This limits play area
namespace layout {
    constexpr int16_t kScoreY = 10;            // Score Y position
    constexpr int16_t kScoreHeight = 32;       // Text height at size 4
    constexpr int16_t kPlayAreaTop = 50;       // Ball/paddle ceiling
    constexpr int16_t kPaddleMargin = 10;      // Distance from screen edge
}

namespace network {
    constexpr const char* kSsid = "Wokwi-GUEST";
    constexpr const char* kPassword = "";
    constexpr const char* kNtpServer = "pool.ntp.org";
    constexpr long kGmtOffsetSec = 10 * 3600;  // AEST = UTC+10
    constexpr int kDaylightOffsetSec = 0;
	}

}  

// ============================================
// COLOR THEME SYSTEM
// ============================================

/**
 * THEME CONFIGURATION GUIDE
 * =========================
 * Each theme defines all colors used in the game.
 * 
 * To change the active theme:
 *   activeTheme = &themes::kGlacier;  // or kCyberpunk, kMonochrome, kMatrix
 * 
 * To create a custom theme:
 *   1. Add a new constexpr Theme in the themes namespace
 *   2. Use RGB565 color values (see color picker tools online)
 * 
 * RGB565 format: 5 bits red, 6 bits green, 5 bits blue
 * Online converter: https://rgbcolorpicker.com/565
 */

/**
 * Theme data structure.
 * 
 * C++ Concept: Aggregate Struct
 * - Can be initialized with brace syntax: {val1, val2, ...}
 * - Members are public by default
 * - No constructors needed for simple initialization
 */
struct Theme {
    uint16_t background;   // Main background color
    uint16_t ball;         // Ball color
    uint16_t paddle;       // Paddle color (base, before flash)
    uint16_t score;        // Score text color
    uint16_t centreLine;   // Dotted centre line
    uint16_t accent;       // Accent for particles/effects
    const char* name;      // Theme name for debugging/UI
};

/**
 * Theme presets.
 * 
 * C++ Concept: constexpr Structs
 * - Entire theme is computed at compile time
 * - Stored in flash memory, saves RAM
 * - Can be pointed to but not modified at runtime
 */
 
namespace themes {

    // Cyberpunk: Neon pink/cyan on deep purple
    constexpr Theme kCyberpunk = {
        0x200A,   // background: deep purple
        0xF81F,   // ball: hot pink (magenta)
        0x07FF,   // paddle: cyan
        0xFFE0,   // score: yellow
        0x4010,   // centreLine: dim purple
        0x07E0,   // accent: neon green (particles)
        "Cyberpunk"
    };
    
    // Glacier: Cool ice blues and whites
    constexpr Theme kGlacier = {
        0x0010,   // background: near black with blue tint
        0xFFFF,   // ball: pure white
        0xAEDC,   // paddle: pale ice blue
        0xC7FF,   // score: light cyan
        0x2104,   // centreLine: dim blue-grey
        0x867F,   // accent: ice blue (particles)
        "Glacier"
    };
    
    // Monochrome: Classic black and white
    constexpr Theme kMonochrome = {
        0x0000,   // background: pure black
        0xFFFF,   // ball: pure white
        0xFFFF,   // paddle: pure white
        0xFFFF,   // score: pure white
        0x4208,   // centreLine: grey
        0xC618,   // accent: light grey (particles)
        "Monochrome"
    };
    
    // Matrix: Green on black, digital rain aesthetic
    constexpr Theme kMatrix = {
        0x0000,   // background: pure black
        0x07E0,   // ball: bright green
        0x07E0,   // paddle: bright green
        0x07E0,   // score: bright green
        0x0320,   // centreLine: dark green
        0x03E0,   // accent: medium green (particles)
        "Matrix"
    };
    
}  // namespace themes

// ============================================
// ACTIVE THEME POINTER
// ============================================

/**
 * Pointer to the currently active theme.
 * 
 * C++ Concept: Pointer to Const
 * - const Theme* means the Theme data cannot be modified
 * - But the pointer itself CAN be changed to point to a different theme
 * 
 * To switch themes at runtime:
 *   activeTheme = &themes::kGlacier;
 */
inline const Theme* activeTheme = &themes::kGlacier;

// Legacy namespace for backward compatibility (uses active theme)
namespace colors {
    // These are kept for any code that still references colors::kBackground etc.
    // They will be replaced by direct theme access in updated code.
    constexpr uint16_t kBackground = 0x0000;  // Fallback, prefer activeTheme->background
    constexpr uint16_t kBall       = 0xFFFF;
    constexpr uint16_t kPaddle     = 0xFFFF;
    constexpr uint16_t kScore      = 0xFFFF;
    constexpr uint16_t kCentreLine = 0x4208;
}

#endif // CONFIG_H