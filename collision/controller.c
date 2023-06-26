//
// Created by pmello on 6/25/23.
//

#include "controller.h"
#include <stdio.h>

void initControllers() {
    InitPAD(&buffer[0][0], 34, &buffer[1][0], 34);
    StartPAD();
    ChangeClearPAD(0);
}

void handlerControllerInput(PADTYPE* controller, Paddle* paddle) {
    if (controller->stat == 0) {

        if ((controller->type == 0x4) || (controller->type == 0x5) || (controller->type == 0x7)) {

            if (!(controller->btn & PAD_UP)) {
                paddle->y = paddle->y  - 2;
            } else if (!(controller->btn & PAD_DOWN)) {
                paddle->y = paddle->y  + 2;
            }
            if (!(controller->btn & PAD_SELECT)) {
                printf("Resetar o PS1\n");
            }
        }
    }


}
