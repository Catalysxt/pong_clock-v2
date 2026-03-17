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

// ============================================
// PARTICLE SYSTEM
// ============================================

/**
 * PARTICLE SYSTEM CONFIGURATION GUIDE
 * ====================================
 * Adjust these values to tweak the goal celebration effect:
 *
 * kMaxParticles (default: 20)
 *   - Maximum particles in the pool
 *   - Higher = more dramatic burst, more CPU/memory
 *   - Range: 10 (subtle) to 40 (explosion)
 *
 * kDefaultParticleLife (default: 30)
 *   - How many frames each particle lives
 *   - Higher = longer trails, particles travel further
 *   - Range: 15 (quick pop) to 60 (lingering sparkle)
 *
 * Particle speed is randomized in burst(), adjust the
 * velocity ranges there for faster/slower particles.
 */

/**
 * Single particle data.
 * 
 * C++ Concept: Plain Old Data (POD) Struct
 * - All public members, no methods
 * - Efficient memory layout for arrays
 * - Uses float for smooth sub-pixel movement
 */
struct Particle {
    float x, y;       // Position (floats for smooth sub-pixel movement)
    float vx, vy;     // Velocity (pixels per frame)
    uint8_t life;     // Frames remaining
    uint8_t maxLife;  // Initial life (for fade calculation)
    uint16_t color;   // Particle color
    bool active;      // Is this slot in use?
};

/**
 * Object pool particle system for goal celebrations.
 * 
 * C++ Concepts:
 * - Object Pool Pattern: Pre-allocate fixed array, reuse "dead" slots
 * - Separation of Concerns: System handles its own update/draw logic
 * - No heap allocation: All particles in stack-allocated array
 */
class ParticleSystem {
public:
    static constexpr uint8_t kMaxParticles = 40;
    static constexpr uint8_t kDefaultParticleLife = 20;
    
    ParticleSystem() {
        // Initialize all particles as inactive
        for (uint8_t i = 0; i < kMaxParticles; i++) {
            particles_[i].active = false;
        }
    }
    
    /**
     * Trigger a burst of particles from a position.
     * Used when a goal is scored.
     * 
     * @param x X-coordinate of burst origin
     * @param y Y-coordinate of burst origin  
     * @param count Number of particles to spawn
     * @param color Base color for particles
     * @param burstRight If true, particles move right; else left
     * 
     * C++ Concept: Object Pool Allocation
     * - Scan for inactive slots and reuse them
     * - No new/delete, no memory fragmentation
     */
    void burst(int16_t x, int16_t y, uint8_t count, uint16_t color, bool burstRight) {
        uint8_t spawned = 0;
        
        for (uint8_t i = 0; i < kMaxParticles && spawned < count; i++) {
            if (!particles_[i].active) {
                particles_[i].active = true;
                particles_[i].x = (float)x;
                particles_[i].y = (float)y + random(-30, 31);  // Spread vertically
                
                // Directional velocity based on which goal was scored
                float baseVx = burstRight ? random(20, 60) / 10.0f : random(-60, -20) / 10.0f;
                particles_[i].vx = baseVx;
                particles_[i].vy = random(-30, 31) / 10.0f;  // Random vertical
                
                particles_[i].life = kDefaultParticleLife + random(-5, 6);
                particles_[i].maxLife = particles_[i].life;
                particles_[i].color = color;
                
                spawned++;
            }
        }
    }
    
    /**
     * Update all active particles.
     * Called once per frame from main loop.
     * 
     * C++ Concept: In-place State Update
     * - Modifies particle array directly
     * - Dead particles marked inactive for reuse
     */
    void update() {
        for (uint8_t i = 0; i < kMaxParticles; i++) {
            if (particles_[i].active) {
                // Apply velocity
                particles_[i].x += particles_[i].vx;
                particles_[i].y += particles_[i].vy;
                
                // Apply gravity (slight downward pull)
                particles_[i].vy += 0.1f;
                
                // Apply drag (slow down over time)
                particles_[i].vx *= 0.98f;
                particles_[i].vy *= 0.98f;
                
                // Decrement life
                particles_[i].life--;
                
                // Deactivate dead particles
                if (particles_[i].life == 0) {
                    particles_[i].active = false;
                }
            }
        }
    }
    
    /**
     * Draw all active particles.
     * 
     * @param display Reference to the TFT display
     * @param bgColor Background color for fading
     */
    template<typename Display>
    void draw(Display& display, uint16_t bgColor) {
        for (uint8_t i = 0; i < kMaxParticles; i++) {
            if (particles_[i].active) {
                // Calculate fade based on remaining life
                float t = 1.0f - ((float)particles_[i].life / (float)particles_[i].maxLife);
                uint16_t fadedColor = lerpColor(particles_[i].color, bgColor, t);
                
                // Draw particle (size decreases with age)
                int16_t size = 3 - (int16_t)(t * 2);
                if (size < 1) size = 1;
                
                display.fillCircle((int16_t)particles_[i].x, (int16_t)particles_[i].y, size, fadedColor);
            }
        }
    }
    
    /**
     * Erase all active particles by drawing background color.
     */
    template<typename Display>
    void erase(Display& display, uint16_t bgColor) {
        for (uint8_t i = 0; i < kMaxParticles; i++) {
            if (particles_[i].active) {
                // Erase with larger radius to ensure clean removal
                display.fillCircle((int16_t)particles_[i].x, (int16_t)particles_[i].y, 4, bgColor);
            }
        }
    }
    
    /**
     * Check if any particles are still active.
     * Useful for knowing when celebration has ended.
     */
    bool hasActiveParticles() const {
        for (uint8_t i = 0; i < kMaxParticles; i++) {
            if (particles_[i].active) return true;
        }
        return false;
    }
    
private:
    Particle particles_[kMaxParticles];  // Fixed-size object pool
};

#endif // EFFECTS_H
