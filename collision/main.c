
#include <psxgpu.h>
#include <psxetc.h>
#include <psxgte.h>
#include <stdio.h>
#include <stdlib.h>
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
char pribuff[2][1024 * 64];
char *nextpri;

struct Coords {
    int x;
    int y;
    int dx;
    int dy;
};

typedef struct {
    int red;
    int green;
    int blue;
} Color;

struct Ball {
    int x;
    int y;
    int w;
    int h;
    int radius;
    Color color;
};

void init(void);
void display();
void retrieve_tim();

int main() {
    SPRT_16 *sprt;
    DR_TPAGE *tpri;
    TILE *tile;

    struct Coords coords;
    struct Ball ball;
    Color color;
    srand(64);
    color.red = (rand() % 256);
    color.green = (rand() % 256);
    color.blue = (rand() %  256);

    ball.w = (rand() % 624);
    ball.h = (rand() % 464);
    ball.x = 1 - (rand() % 3);
    ball.y = 1 - (rand() % 3);
    if (!ball.x) {
        ball.x = 1;
    }
    if (!ball.y) {
        ball.y = 1;
    }

    ball.x *= 2;
    ball.y *= 2;

    ball.color = color;

    coords.x = 0;
    coords.y = 0;
    coords.dx = 2;
    coords.dy = 2;
    init();
    // retrieve_tim();
    VSyncCallback(callback_vsync);
    while (1) {
        ClearOTagR(ot[db], ORDER_TABLE_LEN);
        coords.x += coords.dx;
        coords.y += coords.dy;


        if (coords.x < 0 || coords.x > (SCREEN_XRES - 32)) {
            coords.dx = -coords.dx;
        }

        if (coords.y < 0 || coords.y > (SCREEN_YRES - 32)) {
            coords.dy = -coords.dy;
        }
        FntPrint(-1, "FPS: %d\n", fps_counter.value);
        FntPrint(-1, "X = %d Y = %d\n", coords.x, coords.y);
        FntPrint(-1, "R = %d G = %d B = %d\n", ball.color.red, ball.color.green, ball.color.blue);
        FntFlush(-1);

        tile = (TILE *)nextpri;

        setTile(tile);
        setXY0(tile, coords.x, coords.y);
        setWH(tile, 32, 32);
        setRGB0(tile, ball.color.red, ball.color.green, ball.color.blue);
        addPrim(ot[db], tile);

        nextpri += sizeof(TILE);


        sprt = (SPRT_16 *)nextpri;

        setSprt16(sprt);
        setXY0(sprt, CENTER_X, CENTER_Y);
        setRGB0(sprt, ball.color.red, ball.color.green, ball.color.blue);
        setUV0(sprt, 0, 0);
        setClut(sprt, tim.crect->x, tim.crect->y);

        addPrim(ot[db] + ORDER_TABLE_LEN - 1, sprt);

        nextpri += sizeof(sprt);

        tpri = (DR_TPAGE *)nextpri;
        setDrawTPage(tpri, 0, 0, getTPage(0, 0, tim.prect->x, tim.prect->y));
        addPrim(ot[db] + ORDER_TABLE_LEN - 1, tpri);

        nextpri += sizeof(DR_TPAGE);

        fps_counter.measure_frames++;
        display();


    }

  return 0;
}


void display(void) {
    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    SetDispMask(1);

    DrawOTag(ot[db] + ORDER_TABLE_LEN - 1);
    db = !db;

    nextpri = pribuff[db];
}

void init(void) {
    ResetGraph(0);
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[0], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    // Second buffer
    SetDefDispEnv(&disp[1], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, SCREEN_YRES);

    draw[0].isbg = 1;
    setRGB0(&draw[0], 0, 0, 0);
    draw[1].isbg = 1;
    setRGB0(&draw[1], 0, 0, 0);

    PutDispEnv(&disp[0]);
    PutDispEnv(&disp[1]);

    db = 0;
    nextpri = pribuff[0]; // Set initial primitive pointer address
    FntLoad(960, 256);
    FntOpen(0, 8, 320, 224, 0, 100);
}

//void retrieve_tim() {
//    printf("Upload texture...");
//    GetTimInfo(ball_tile, &tim);
//
//    LoadImage(tim.prect, tim.paddr);
//    if (tim.mode & 0x8) {
//        LoadImage(tim.crect, tim.caddr);
//    }
// }