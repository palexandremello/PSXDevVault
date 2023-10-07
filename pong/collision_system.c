
#include "collision_system.h"

#define FIXED_POINT_SHIFT 12



int detectCollision(Paddle *player, Coords *coords, Ball *ball) {
    return (player->x < coords->x + ball->w &&
            player->x + player->w > coords->x &&
            player->y < coords->y + ball->h &&
            player->y + player->h > coords->y);
}

void handleCollision(Paddle *player, Coords *coords, Ball *ball, int factor) {


    int middleY = player->y + (player->h / 2);
    int ballMiddleY = coords->y + (ball->h / 2);

    int middleX = player->x + (player->w / 2);
    int ballMiddleX = coords->y + (ball->h / 2);

    if (ballMiddleY < middleY || ballMiddleY > middleY) {
        coords->dy = -coords->dy;
    }

    if (ballMiddleX < middleX  || ballMiddleX > middleX) {
        coords->dx = -coords->dx;
    }

    int distanceX = ballMiddleX - middleX;
    int distanceY = ballMiddleY - middleY;

    int new_factor = factor / 2;

    coords->dx += (distanceX * new_factor) >> FIXED_POINT_SHIFT;
    coords->dy += (distanceY * new_factor) >> FIXED_POINT_SHIFT;

};
