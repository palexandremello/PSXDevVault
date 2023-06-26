//
// Created by pmello on 6/25/23.
//
#include <psxpad.h>

typedef struct {
    PADTYPE pad;
} PlayerController;

#ifndef CONTROLLER_H
#define CONTROLLER_H

void controller_init(void);

extern PlayerController player_controller;
#endif //CONTROLLER_H
