/*
  Visual effects utilities for Pong Clock
  Contains: Color interpolation, easing functions
*/

#ifndef EFFECTS_H
#define EFFECTS_H

#include <cstdint>

// ============================================
// COLOR UTILITIES
// ============================================

/**
 * Linearly interpolate between two RGB565 colors.
 * 
 * RGB565 format: RRRRRGGGGGGBBBBB (5 bits red, 6 bits green, 5 bits blue)
 * 
 * @param colorA Starting color (when t = 0.0)
 * @param colorB Ending color (when t = 1.0)
 * @param t Interpolation factor (0.0 to 1.0)
 * @return Interpolated RGB565 color
 * 
 * C++ Concepts:
 * - Bitwise AND (&) to mask out specific bits
 * - Bit shifting (>> and <<) to extract/position color channels
 * - Inline function hint for compiler optimization
 */
inline uint16_t lerpColor(uint16_t colorA, uint16_t colorB, float t) {
    // Clamp t to valid range
    if (t <= 0.0f) return colorA;
    if (t >= 1.0f) return colorB;
    
    // Extract RGB components from colorA (source)
    // Red:   bits 15-11 (5 bits)
    // Green: bits 10-5  (6 bits)
    // Blue:  bits 4-0   (5 bits)
    uint8_t r1 = (colorA >> 11) & 0x1F;  // 0x1F = 0b11111 (5 bits)
    uint8_t g1 = (colorA >> 5)  & 0x3F;  // 0x3F = 0b111111 (6 bits)
    uint8_t b1 =  colorA        & 0x1F;
    
    // Extract RGB components from colorB (target)
    uint8_t r2 = (colorB >> 11) & 0x1F;
    uint8_t g2 = (colorB >> 5)  & 0x3F;
    uint8_t b2 =  colorB        & 0x1F;
    
    // Linearly interpolate each channel: result = a + t * (b - a)
    uint8_t r = r1 + (int16_t)((r2 - r1) * t);
    uint8_t g = g1 + (int16_t)((g2 - g1) * t);
    uint8_t b = b1 + (int16_t)((b2 - b1) * t);
    
    // Repack into RGB565 format
    return (r << 11) | (g << 5) | b;
}

// ============================================
// EASING FUNCTIONS
// ============================================

/**
 * Quadratic ease-out: fast start, slow end.
 * Creates a more natural-feeling deceleration.
 * 
 * @param t Input value (0.0 to 1.0)
 * @return Eased output value
 */
inline float easeOutQuad(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}

/**
 * Quadratic ease-in: slow start, fast end.
 * 
 * @param t Input value (0.0 to 1.0)
 * @return Eased output value
 */
inline float easeInQuad(float t) {
    return t * t;
}

#endif // EFFECTS_H
