//
// Created by pmello on 10/1/23.
//

#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "controller.h"
#include "coords.h"
#include "ball.h"

int detectCollision(Paddle *player, Coords *coords, Ball *ball);
void handleCollision(Paddle *player, Coords *coords, Ball *ball, int factor);
#endif //COLLISION_SYSTEM_H
