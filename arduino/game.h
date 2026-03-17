#pragma once

#include "game_objects.h"

extern Ball ball;
extern Paddle LeftPaddle;
extern Paddle RightPaddle;

extern int scoreLeft;
extern int scoreRight;

void resetBall();
void gameInit();
void updateBall();
void updatePaddles();
void gameUpdate();
