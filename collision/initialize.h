#include <stdio.h>
typedef struct  {
    volatile int value;
    volatile int counter;
    volatile int measure_frames;
} FPSCounter;

extern FPSCounter fps_counter;

void callback_vsync(void);
