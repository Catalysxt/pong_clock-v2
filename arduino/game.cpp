#include "game.h"
#include "config.h"
#include "display.h"
#include "game_objects.h"

Ball ball(
    hw::display::kWidth / 2,
    hw::display::kHeight / 2,
    BALL_R
);

Paddle LeftPaddle(
    LEFT_X,
    hw::display::kHeight / 2 - PADDLE_H / 2,
    PADDLE_W,
    PADDLE_H
);

Paddle RightPaddle(
    RIGHT_X,
    hw::display::kHeight / 2 - PADDLE_H / 2,
    PADDLE_W,
    PADDLE_H
);

int scoreLeft  = 0;
int scoreRight = 0;

void resetBall() {
    ball = Ball(
        hw::display::kWidth / 2,
        hw::display::kHeight / 2,
        BALL_R
    );

    float vx = random(0, 2) ? 3 : -3;
    float vy = random(-2, 3);

    if (vy == 0) {
        vy = 1;
    }

    ball.setvel(vx, vy);
}

void gameInit() {
    resetBall();
    drawCentreLine();
    drawScore(scoreLeft, scoreRight);
    LeftPaddle.draw();
    RightPaddle.draw();
    ball.draw();
}

void updatePaddles() {
    LeftPaddle.updateAI(ball.pos().y);
    RightPaddle.updateAI(ball.pos().y);
}

void updateBall() {
    ball.erase();

    Vec2 pos = ball.pos();
    Vec2 vel = ball.vel();

    pos.x += vel.x;
    pos.y += vel.y;

    if (pos.y <= BALL_R || pos.y >= hw::display::kHeight - BALL_R) {
        vel.y = -vel.y;
    }

    Vec2 lp = LeftPaddle.pos();
    if (pos.x <= LEFT_X + PADDLE_W &&
        pos.y >= lp.y &&
        pos.y <= lp.y + PADDLE_H) {
        vel.x = -vel.x;
        pos.x = LEFT_X + PADDLE_W + BALL_R;
    }

    Vec2 rp = RightPaddle.pos();
    if (pos.x >= RIGHT_X - BALL_R &&
        pos.y >= rp.y &&
        pos.y <= rp.y + PADDLE_H) {
        vel.x = -vel.x;
        pos.x = RIGHT_X - BALL_R;
    }

    if (pos.x < 0) {
        scoreRight++;
        drawScore(scoreLeft, scoreRight);
        resetBall();
        return;
    }

    if (pos.x > hw::display::kWidth) {
        scoreLeft++;
        drawScore(scoreLeft, scoreRight);
        resetBall();
        return;
    }

    ball = Ball((int)pos.x, (int)pos.y, BALL_R);
    ball.setvel(vel.x, vel.y);
    ball.draw();
}

void gameUpdate() {
    updateBall();
    updatePaddles();
}
