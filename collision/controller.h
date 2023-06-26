//
// Created by pmello on 6/25/23.
//
#include <psxpad.h>

typedef struct {
    PADTYPE pad;
} PlayerPad;

typedef struct {
    char buffer[2][34];
} Controller;

#ifndef CONTROLLER_H
#define CONTROLLER_H

void controller_init(void);
void init_buffer_controller(void );

extern Controller controller;
extern PlayerPad player_pad;
#endif //CONTROLLER_H
