#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <cstdint>
#include <Adafruit_ILI9341.h>
#include "config.h"
#include "effects.h"


// ============================================
// GAME OBJECTS
// ============================================

struct Vec2 {
	int16_t x;
	int16_t y;
};

class Ball {

	public:
		// Trail configuration - static constexpr means compile-time constant
		static constexpr uint8_t kTrailLength = 8;   // Number of ghost positions
		static constexpr uint8_t kTrailMinRadius = 2; // Smallest trail dot radius
		
		Ball(uint16_t x, uint16_t y, int16_t rad) : // Constructor with member init list 
		 pos_{x, y}, vel_{x, y}, rad_(rad), trailHead_(0), trailCount_(0) {
			// Initialize trail history to starting position
			for (uint8_t i = 0; i < kTrailLength; i++) {
				trailHistory_[i] = pos_;
			}
		}
	
	void getUpdate() {
		// Store current position in trail BEFORE moving
		// This creates the "history" of where the ball was
		storeTrailPosition();
		
		// Motion = Current position + velocity
		pos_.x += vel_.x;
		pos_.y += vel_.y;
		
		// Recall that the top of the screen is y = 0
		// Bounce off play ceiling
		if (pos_.y <= hw::layout::kPlayAreaTop + rad_) {
			pos_.y = hw::layout::kPlayAreaTop + rad_; // Clamp to boundary
			vel_.y = -vel_.y; // Reverse direction
		}
		
		// Bounce off bottom edge
		// Recall that the bottom of the screen is y = 240
		if (pos_.y >= hw::display::kHeight - rad_) {
			pos_.y = hw::display::kHeight - rad_; // Clamp to boundary
			vel_.y = -vel_.y; // Reverse direction
		}
		
		// TODO: Bounce off paddles (left/right)
		
	}
	
	/**
	 * Store current position in the circular buffer.
	 * 
	 * C++ Concept: Circular Buffer (Ring Buffer)
	 * - trailHead_ is the write index, always moves forward
	 * - Modulo (%) wraps the index back to 0 when it reaches kTrailLength
	 * - This gives us FIFO behavior without shifting array elements
	 */
	void storeTrailPosition() {
		trailHistory_[trailHead_] = pos_;
		trailHead_ = (trailHead_ + 1) % kTrailLength;  // Wrap around
		
		// Warm-up: gradually fill the trail buffer
		if (trailCount_ < kTrailLength) {
			trailCount_++;
		}
	}
	
	/**
	 * Draw the trail (ghost positions) from oldest to newest.
	 * Older positions are more faded and smaller.
	 * 
	 * @param display Reference to the TFT display
	 * @param ballColor The main ball color (trail fades from this to background)
	 */
	void drawTrail(Adafruit_ILI9341& display, uint16_t ballColor) {
		// Draw from oldest to newest (so newest paints on top if overlapping)
		for (uint8_t i = 0; i < trailCount_; i++) {
			// Calculate the actual index in our circular buffer
			// Start from (trailHead_ - trailCount_) and go forward
			// Add kTrailLength before modulo to handle negative wrap
			uint8_t idx = (trailHead_ + kTrailLength - trailCount_ + i) % kTrailLength;
			
			// Calculate age: 0 = oldest, trailCount_-1 = newest
			uint8_t age = i;
			
			// Calculate fade factor: 0.0 (newest, full color) to 1.0 (oldest, faded)
			float t = 1.0f - ((float)age / (float)trailCount_);
			
			// Apply ease-out for smoother fade appearance
			t = easeOutQuad(t);
			
			// Interpolate color from ball color to background
			uint16_t ghostColor = lerpColor(ballColor, colors::kBackground, t);
			
			// Interpolate radius from full size to minimum
			int16_t ghostRad = kTrailMinRadius + (int16_t)((rad_ - kTrailMinRadius) * (1.0f - t));
			
			// Don't draw if too small or fully faded
			if (ghostRad >= kTrailMinRadius && t < 0.95f) {
				display.fillCircle(trailHistory_[idx].x, trailHistory_[idx].y, ghostRad, ghostColor);
			}
		}
	}
	
	/**
	 * Erase the trail by drawing over old positions with background color.
	 */
	void eraseTrail(Adafruit_ILI9341& display) {
		for (uint8_t i = 0; i < trailCount_; i++) {
			uint8_t idx = (trailHead_ + kTrailLength - trailCount_ + i) % kTrailLength;
			// Erase with slightly larger radius to ensure clean removal
			display.fillCircle(trailHistory_[idx].x, trailHistory_[idx].y, rad_ + 1, colors::kBackground);
		}
	}
	
	/**
	 * Reset trail when ball is reset (e.g., after scoring)
	 */
	void resetTrail() {
		trailHead_ = 0;
		trailCount_ = 0;
		for (uint8_t i = 0; i < kTrailLength; i++) {
			trailHistory_[i] = pos_;
		}
	}
	
	void draw(Adafruit_ILI9341& display, uint16_t color) {
		display.fillCircle(pos_.x, pos_.y, rad_, color);
	}
	
	void erase(Adafruit_ILI9341& display) {
		draw(display, colors::kBackground);
	}
	
   // Accessors
   Vec2 pos() const { return pos_; }
   Vec2 vel() const { return vel_; }
   int16_t rad() const { return rad_; }
	
	// Mutators (For game logic to manipulate ball)
	void setvel(uint16_t vx, uint16_t vy) { vel_ = {vx, vy}; }
	
	private:
		Vec2 pos_; // Current position (pixels)
		Vec2 vel_; // Velocity (pixels per frame)
		int16_t rad_;
		
		// Trail system - Circular buffer for position history
		Vec2 trailHistory_[kTrailLength];  // Fixed-size array (no heap allocation)
		uint8_t trailHead_;                 // Current write index (0 to kTrailLength-1)
		uint8_t trailCount_;                // Warm-up counter (0 to kTrailLength)

};

class Paddle {
	public:
		// Constructor
		Paddle(int16_t x, int16_t y, int16_t w, int16_t h) :
		pos_{x, y}, width_(w), height_(h), speed_(3),
    reactionDelay_(0), targetY_(y), lastUpdate_(0) {}

	void updateAI(int16_t ballY, int16_t ballX, bool isLeftPaddle) {
    unsigned long now = millis();
    
    // Only update target occasionally (reaction time)
    if (now - lastUpdate_ > reactionDelay_) {
        lastUpdate_ = now;
        
        // Set new reaction delay (50-150ms, like human reflexes)
        reactionDelay_ = random(50, 150);
        
        // Predict where to go, but with some error
        int16_t error = random(-15, 16);
        targetY_ = ballY + error;
        
        // Occasionally "lose focus" and drift toward center
        if (random(0, 10) == 0) {
            targetY_ = hw::display::kHeight / 2;
        }
    }
    
    // Move toward target (not instant, gradual)
    int16_t centerY = pos_.y + height_ / 2;
    int16_t diff = targetY_ - centerY;
    
    // Add "laziness" - don't move for tiny adjustments
    if (abs(diff) > 5) {
        // Clamp movement to speed limit (avoid overshoot)
        int16_t movement = diff;
        if (movement > static_cast<int16_t>(speed_)) {
            movement = static_cast<int16_t>(speed_);
        } else if (movement < -static_cast<int16_t>(speed_)) {
            movement = -static_cast<int16_t>(speed_);
        }
        pos_.y += movement;
    } else {
        // Small random fidgeting when "waiting"
        if (random(0, 5) == 0) {
            pos_.y += random(-2, 3);
        }
    }
    // Clamp to play area
    if (pos_.y < hw::layout::kPlayAreaTop) {
        pos_.y = hw::layout::kPlayAreaTop;
    }
    if (pos_.y > hw::display::kHeight - height_) {
        pos_.y = hw::display::kHeight - height_;
    }
}
	
	
		void moveToward(int16_t targetY) {
			int16_t centerY = pos_.y + height_ / 2;

			if (centerY < targetY - 2) {
					pos_.y += speed_;
			} else if (centerY > targetY + 2) {
					pos_.y -= speed_;
			}
			
			// Clamp to play area (below score)
			if (pos_.y < hw::layout::kPlayAreaTop) {
					pos_.y = hw::layout::kPlayAreaTop;
			}
			if (pos_.y > hw::display::kHeight - height_) {
					pos_.y = hw::display::kHeight - height_;
			}
}
		
		// Collision detection. Has the ball made contact with the paddle?
		bool contain(uint16_t x, uint16_t y) const {
		// Take into account the thickness of the paddle for collison detection
			return x >= pos_.x && x <= pos_.x + width_ &&
						 y >= pos_.y && y <= pos_.x + height_;
		}
		
    // The paddle is drawn using the arguments via pass by value
		void draw(Adafruit_ILI9341& display, uint16_t color) {
	    display.fillRect(pos_.x, pos_.y, width_, height_, color);
    }
    
    void erase(Adafruit_ILI9341& display) {
        draw(display, colors::kBackground);
    }
		
	// Accessors
	Vec2 pos() const { return pos_; }
	int16_t width() const { return width_; }
	int16_t height() const { return height_; }

	
	private:
	Vec2 pos_;
	int16_t width_;
	int16_t height_;
	int16_t speed_;

	// AI state
  unsigned long reactionDelay_;
  int16_t targetY_;
  unsigned long lastUpdate_;

};

#endif GAME_OBJECTS_H