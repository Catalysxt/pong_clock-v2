#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// For NTP Time Sync
#include "time_sync.h"

#include "config.h"
#include "game_objects.h"

// ============================================
// DISPLAY INSTANCE
// ============================================
Adafruit_ILI9341 tft(
    hw::display::pins::kCS,
    hw::display::pins::kDC,
    hw::display::pins::kRST
);

// ============================================
// DRAWING HELPERS
// ============================================

void drawCentreLine() {
	const uint16_t x = hw::display::kWidth / 2;
	for (uint16_t y = 0; y < hw::display::kHeight; y+=10) {
		tft.fillRect(x - 1, y, 2, 5, activeTheme->centreLine);
	}
}

void drawBall(uint16_t x, uint16_t y, uint16_t rad, uint16_t color) {
	tft.fillCircle(x, y, rad, color);
}

void drawPaddle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

void drawScore(int leftScore, int rightScore) {
    tft.setTextColor(activeTheme->score, activeTheme->background);
    tft.setTextSize(4);
    
    tft.setCursor(100, 10);
    if (leftScore < 10) tft.print("0");
    tft.print(leftScore);
    
    tft.setCursor(148, 10);
    tft.print(":");
    
    tft.setCursor(172, 10);
    if (rightScore < 10) tft.print("0");
    tft.print(rightScore);
}

// ============================================
// GAME OBJECT INSTANCES
// ============================================

// Create game objects
Ball ball(hw::display::kWidth / 2, hw::display::kHeight / 2, 5);
Paddle LeftPaddle(10, hw::layout::kPlayAreaTop + 50, 10, 40);
Paddle RightPaddle(300, hw::layout::kPlayAreaTop + 50, 10, 40);

int LeftScore = 0;
int RightScore = 0;
bool gameRunning = true;

// Particle system for goal celebrations
ParticleSystem goalParticles;

// Goal celebration state (non-blocking animation)
unsigned long goalCelebrationEndTime = 0;  // millis() timestamp when celebration ends
bool isGoalCelebrating = false;

// Forward Declarations

void resetBall();
void UpdateScoreDisplay();

// ============================================
// HELPPER FUNCTIONS FOR GAME LOGIC
// ============================================

void checkPaddleCollision() {
	Vec2 ballPos = ball.pos();
	int16_t rad = ball.rad();

	// Check collision with left paddle
	if (ballPos.x - rad <= LeftPaddle.pos().x + LeftPaddle.width() &&
		ballPos.x - rad >= LeftPaddle.pos().x &&
		ballPos.y >= LeftPaddle.pos().y &&
		ballPos.y <= LeftPaddle.pos().y + LeftPaddle.height()) {

	// Reverse x direction and introduce randomness 
	Vec2 currentVel = ball.vel();
	ball.setvel(abs(currentVel.x) + random(1, 1), currentVel.y + random(-2, 2));
	
	// Trigger paddle flash on impact
	LeftPaddle.triggerFlash();

	}	

    // Check collision with right paddle
    if (ballPos.x + rad >= RightPaddle.pos().x &&
        ballPos.x + rad <= RightPaddle.pos().x + RightPaddle.width() &&
        ballPos.y >= RightPaddle.pos().y &&
        ballPos.y <= RightPaddle.pos().y + RightPaddle.height()) {
        
        // Reverse X direction and add slight randomness
        Vec2 currentVel = ball.vel();
        ball.setvel(-abs(currentVel.x) + random(-1, 1), currentVel.y + random(-2, 2));
        
        // Trigger paddle flash on impact
        RightPaddle.triggerFlash();
    }
}

void checkGoal() {
	// Don't check for goals during celebration
	if (isGoalCelebrating) return;

	Vec2 ballPos = ball.pos();
	int16_t rad = ball.rad();

	// Determine right player made a goal
	if (ballPos.x - rad <= 0)
	{
		RightScore++;
		// Burst particles from left edge, moving right
		goalParticles.burst(0, ballPos.y, 15, activeTheme->accent, true);
		// Start non-blocking celebration
		isGoalCelebrating = true;
		goalCelebrationEndTime = millis() + 1600;  // 1600ms celebration
		// Erase and reset ball immediately (no blocking delay)
		ball.eraseTrail(tft);
		ball.erase(tft);
		ball = Ball(hw::display::kWidth / 2, hw::display::kHeight / 2, 3);
		int dirX = random(0, 2) == 0 ? 1 : -1;
		ball.setvel(dirX, random(-2, 2));
	}

	// Determine left player made a goal
	if (ballPos.x + rad >= hw::display::kWidth)
	{
		LeftScore++;
		// Burst particles from right edge, moving left
		goalParticles.burst(hw::display::kWidth, ballPos.y, 15, activeTheme->accent, false);
		// Start non-blocking celebration
		isGoalCelebrating = true;
		goalCelebrationEndTime = millis() + 1600;  // 1600ms celebration
		// Erase and reset ball immediately (no blocking delay)
		ball.eraseTrail(tft);
		ball.erase(tft);
		ball = Ball(hw::display::kWidth / 2, hw::display::kHeight / 2, 3);
		int dirX = random(0, 2) == 0 ? 1 : -1;
		ball.setvel(dirX, random(-2, 2));
	}
}

void resetBall() {
	// Erase old ball and trail before resetting
	ball.eraseTrail(tft);
	ball.erase(tft);
	
	// Reset ball at center with new random velocity
	ball = Ball(hw::display::kWidth / 2, hw::display::kHeight / 2, 3);

	// Random direction with random velocity
	int dirX = random(0, 2) == 0 ? 1 : -1;
	ball.setvel(dirX, random(-2, 2));

	ball.draw(tft, activeTheme->ball);

	// Brief pause after goal
	delay(1000);

}

void UpdateScoreDisplay() {

	tft.fillScreen(activeTheme->background);
	drawScore(LeftScore, RightScore);
	drawCentreLine();
}

void updatePaddleAI() {
  Vec2 ballPos = ball.pos();
    
  LeftPaddle.updateAI(ballPos.y, ballPos.x, true);
	RightPaddle.updateAI(ballPos.y, ballPos.x, false);
}


// ============================================
// SETUP
// ============================================
void setup() {
    Serial.begin(115200);
    Serial.println("=== Pong Clock Starting ===");
    
    // Initialize display
    tft.begin();
    tft.setRotation(1);  // Landscape mode
    tft.fillScreen(activeTheme->background);

    // Show "Connecting..." on display
    tft.setTextColor(activeTheme->score);
    tft.setTextSize(2);
    tft.setCursor(80, 110);
    tft.print("Connecting...");

    // Begin NTP Time Sync
    TimeSync::init();

    // Set initial score from current time
    LeftScore = TimeSync::getHour();
    RightScore = TimeSync::getMinute();

    // Draw game
    tft.fillScreen(activeTheme->background);
    drawCentreLine();
    drawScore(LeftScore, RightScore);

	// Draw game objects
	ball.draw(tft, activeTheme->ball);
	LeftPaddle.draw(tft, activeTheme->paddle);
	RightPaddle.draw(tft, activeTheme->paddle);

	ball.setvel(4, 1); // Start moving diagonally 

	Serial.println("=== Pong Ready ===");
    Serial.println("=== May the Best Play Win! ===");
    Serial.printf("Target time: %02d:%02d\n", LeftScore, RightScore);
}
// ============================================
// MAIN LOOP
// ============================================
void loop() {

	if (!gameRunning)
	{
		return;
	}

    // Update time (non-blocking, cached internally)
    TimeSync::update();

    // Check if we need to adjust scores to match time
    int targetHour = TimeSync::getHour();
    int targetMinute = TimeSync::getMinute();

	// 1. Erase old positions (particles, trail, ball, paddles)
	goalParticles.erase(tft, activeTheme->background);
	ball.eraseTrail(tft);
	ball.erase(tft);
	LeftPaddle.erase(tft);
	RightPaddle.erase(tft);

	// 2. Update positions
	goalParticles.update();  // Always update particles
	
	// Check if celebration has ended
	if (isGoalCelebrating && millis() >= goalCelebrationEndTime) {
		isGoalCelebrating = false;
		UpdateScoreDisplay();  // Refresh screen after particles fade
	}
	
	// Only update game objects if not celebrating
	if (!isGoalCelebrating) {
		ball.getUpdate();
		updatePaddleAI();
		checkPaddleCollision();
		checkGoal();
	}

	// 3. Draw game objects (particles first, then trail, then ball on top)
	goalParticles.draw(tft, activeTheme->background);
	ball.drawTrail(tft, activeTheme->ball);
    ball.draw(tft, activeTheme->ball);
    LeftPaddle.draw(tft, LeftPaddle.getCurrentColor(activeTheme->paddle));
    RightPaddle.draw(tft, RightPaddle.getCurrentColor(activeTheme->paddle));
    
    // 4. Small delay for ~60 FPS (16ms per frame)
    delay(16);

}