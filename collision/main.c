
#include <psxgpu.h>
#include <psxetc.h>
#include <psxgte.h>
#include "initialize.h"


#define ORDER_TABLE_LEN 8
#define SCREEN_XRES 320
#define SCREEN_YRES 240

#define CENTER_X SCREEN_XRES / 2
#define CENTER_Y SCREEN_YRES / 2

DISPENV disp[2];
DRAWENV draw[2];
int db = 0;

uint32_t ot[2][ORDER_TABLE_LEN];
char pribuff[2][32768];
char *nextpri;

struct Coords {
    int x;
    int y;
    int dx;
    int dy;
};

void init(void);
void display();

int main() {
    TILE *tile1;
    struct Coords coords;

    coords.x = 0;
    coords.y = 0;
    coords.dx = 2;
    coords.dy = 2;

    init();
    VSyncCallback(callback_vsync);
    while (1) {
        coords.x += coords.dx;
        coords.y += coords.dy;


        if (coords.x < 0 || coords.x > (SCREEN_XRES - 32)) {
            coords.dx = -coords.dx;
        }

        if (coords.y < 0 || coords.y > (SCREEN_YRES - 32)) {
            coords.dy = -coords.dy;
        }
        ClearOTagR(ot[db], ORDER_TABLE_LEN);
        FntPrint(-1, "FPS: %d\n", fps_counter.value);

        tile1 = (TILE *)nextpri;

        setTile(tile1);
        setXY0(tile1, coords.x, coords.y);
        setWH(tile1, 32, 32);
        setRGB0(tile1, 255, 255, 0);
        addPrim(ot[db], tile1); // Add primitive to the ordering table

        nextpri += sizeof(TILE);


        printf("FPS: %d\n", fps_counter.value);
        fps_counter.measure_frames++;
        display();
        FntFlush(-1);


    }

  return 0;
}


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
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[0], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    // Second buffer
    SetDefDispEnv(&disp[1], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, SCREEN_YRES);

    draw[0].isbg = 1;
    setRGB0(&draw[0], 63, 0, 120);
    draw[1].isbg = 1;
    setRGB0(&draw[1], 63, 0, 120);

    nextpri = pribuff[0];


    FntLoad(960, 256);
    FntOpen(0, 8, 320, 224, 0, 100);
}