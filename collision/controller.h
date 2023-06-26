//
// Created by pmello on 6/25/23.
//
#include <psxpad.h>
#include <psxapi.h>

#define NUM_CONTROLLERS 2

#ifndef CONTROLLER_H
#define CONTROLLER_H

extern char buffer[NUM_CONTROLLERS][34];

typedef struct {
    int x;
    int y;
    int w;
    int h;
    int hh;
    int hw;
} Paddle;


void handlerControllerInput(PADTYPE* controller, Paddle* paddle);
void initControllers();
#endif //CONTROLLER_H
