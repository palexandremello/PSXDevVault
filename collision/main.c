
#include <inline_c.h>
#include <psxapi.h>
#include <psxetc.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <psxpad.h>
#include <sys/types.h>
#include "initialize.h"


#define ORDER_TABLE_LEN 8
#define SCREEN_XRES 640
#define SCREEN_YRES 480

#define CENTER_X SCREEN_XRES / 2
#define CENTER_Y SCREEN_YRES / 2

DISPENV disp[2];
DRAWENV draw[2];

int db = 0;

u_long ot[2][ORDER_TABLE_LEN];
char pribuff[2][32768];
char *nextpri;

void display() {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    SetDispMask(1);
    DrawOTag(ot[db] + ORDER_TABLE_LEN - 1);
    db = !db;
    nextpri = pribuff[db];

};

void init(void) {
    ResetGraph(0);
    SetDefDispEnv(&disp[0], 0, 0, CENTER_X, CENTER_Y);
    SetDefDrawEnv(&draw[0], 0, CENTER_Y, CENTER_X, CENTER_Y);
    // Second buffer
    SetDefDispEnv(&disp[1], 0, CENTER_Y, CENTER_X, CENTER_Y);
    SetDefDrawEnv(&draw[1], 0, 0, CENTER_X, CENTER_Y);

    draw[0].isbg = 1;
    setRGB0(&draw[0], 63, 0, 120);
    draw[1].isbg = 1;
    setRGB0(&draw[1], 63, 0, 120);

    nextpri = pribuff[0];


    FntLoad(960, 256);
    FntOpen(0, 8, 320, 224, 0, 100);
}
int main() {
    init();
    VSyncCallback(callback_vsync);
    while (1) {
        ClearOTagR(ot[db], ORDER_TABLE_LEN);
        FntPrint(-1, "FPS: %d\n", fps_counter.value);
        FntFlush(-1);
        printf("FPS: %d\n", fps_counter.value);
        fps_counter.measure_frames++;
        display();


    }

  return 0;
}


