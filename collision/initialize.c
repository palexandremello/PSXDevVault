//
// Created by pmello on 6/24/23.
//

#include "initialize.h"

FPSCounter fps_counter;

void callback_vsync(void) {
    (fps_counter.counter)++;
    if (fps_counter.counter >= 60) {
        fps_counter.value = fps_counter.measure_frames;
        fps_counter.measure_frames = 0;
        fps_counter.counter = 0;
    }

}