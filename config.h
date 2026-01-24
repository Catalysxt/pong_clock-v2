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

namespace colors {
	constexpr uint16_t kBackground = ILI9341_BLACK;
	constexpr uint16_t kBall       = ILI9341_WHITE;
	constexpr uint16_t kPaddle     = ILI9341_WHITE;
	constexpr uint16_t kScore      = ILI9341_WHITE;
	constexpr uint16_t kCentreLine =  0x4208;
}

#endif CONFIG_H