#pragma once

#include "config.h"
#include "display.h"

struct Vec2 {

    float x;
    float y;

};

class Ball {

private:

    Vec2 position;
    Vec2 velocity;
    int radius;

public:

    Ball(int x, int y, int r)
        : position{(float)x,(float)y}, velocity{2,1}, radius(r) {}

    Vec2 pos() const { return position; }
    Vec2 vel() const { return velocity; }

    int rad() const { return radius; }

    void setvel(float vx, float vy) {

        velocity.x = vx;
        velocity.y = vy;

    }

    void getUpdate() {

        position.x += velocity.x;
        position.y += velocity.y;

        if (position.y <= hw::layout::kPlayAreaTop + radius ||
            position.y >= hw::display::kHeight - radius) {

            velocity.y = -velocity.y;

        }

    }

    void draw() {

        drawBall(position.x, position.y);

    }

    void erase() {

        eraseBall(position.x, position.y);

    }

};

class Paddle {

private:

    Vec2 position;
    int width;
    int height;

public:

    Paddle(int x, int y, int w, int h)
        : position{(float)x,(float)y}, width(w), height(h) {}

    Vec2 pos() const { return position; }

    int widthPx() const { return width; }
    int heightPx() const { return height; }

    void draw() {

        drawPaddle(position.x, position.y);

    }

    void erase() {

        erasePaddle(position.x, position.y);

    }

    void updateAI(float ballY) {

        erase();

        if (ballY < position.y + height/2)
            position.y -= 3;

        if (ballY > position.y + height/2)
            position.y += 3;

        position.y = constrain(
            position.y,
            hw::layout::kPlayAreaTop,
            hw::display::kHeight - height
        );

        draw();

    }

};
