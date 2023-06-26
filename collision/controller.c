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

void handlerControllerInput(PADTYPE* controller) {
    printf("%s", controller);
    if (controller->stat == 0) {

        if ((controller->type == 0x4) || (controller->type == 0x5) || (controller->type == 0x7)) {

            if (!(controller->btn & PAD_UP)) {
               printf("Subir o paddle\n");
            } else if (!(controller->btn & PAD_DOWN)) {
               printf("Descer o Paddle\n");
            }
            if (!(controller->btn & PAD_SELECT)) {
                printf("Restar o PS1\n");
            }
        }
    }


}
