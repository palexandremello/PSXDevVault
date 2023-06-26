//
// Created by pmello on 6/25/23.
//
#include <psxpad.h>
#include <psxapi.h>

#define NUM_CONTROLLERS 2

#ifndef CONTROLLER_H
#define CONTROLLER_H

extern char buffer[NUM_CONTROLLERS][34];

void handlerControllerInput(PADTYPE* controller);
void initControllers();
#endif //CONTROLLER_H
