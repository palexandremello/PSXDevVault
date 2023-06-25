
#include <psxgpu.h>
#include <psxetc.h>
#include <psxgte.h>
#include <stdio.h>
#include <stdlib.h>
#include "initialize.h"


#define OT_LEN 8
#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define MARGINX 32      // margins for text display
#define MARGINY 44
#define FONTSIZE 8 * 3          // Text Field Height
#define CENTER_X SCREEN_XRES / 2
#define CENTER_Y SCREEN_YRES / 2

DISPENV disp[2];
DRAWENV draw[2];
int db = 0;

uint32_t ot[2][OT_LEN];
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

extern const uint32_t  ball_tile[];

TIM_IMAGE tim;

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
    coords.dx = 1;
    coords.dy = 1;
    init();
    VSyncCallback(callback_vsync);
    while (1) {
        ClearOTagR(ot[db], OT_LEN);

        coords.x += coords.dx;
        coords.y += coords.dy;


        if (coords.x < 0 || coords.x > (SCREEN_XRES - 16)) {
            coords.dx = -coords.dx;
        }

        if (coords.y < 0 || coords.y > (SCREEN_YRES - 16)) {
            coords.dy = -coords.dy;
        }
        FntPrint(-1, "FPS: %d\n", fps_counter.value);
        FntPrint(-1, "X = %d Y = %d\n", coords.x, coords.y);
        FntPrint(-1, "R = %d G = %d B = %d\n", ball.color.red, ball.color.green, ball.color.blue);
        FntPrint(-1, "CLUT (X, Y) = (%d, %d)\n", tim.crect->x, tim.crect->y);

        sprt = (SPRT_16 *)nextpri;

        setSprt16(sprt);
        setXY0(sprt, coords.x, coords.y);
        setRGB0(sprt, ball.color.red, ball.color.green, ball.color.blue);
        setClut(sprt, tim.crect->x, tim.crect->y);
        addPrim(ot[db], sprt);

        nextpri += sizeof(SPRT_16);
        tpri = (DR_TPAGE *)nextpri;
        setDrawTPage(tpri, 0, 0, getTPage(0, 0, tim.prect->x, tim.prect->y));
        addPrim(ot[db], tpri);
        nextpri += sizeof(DR_TPAGE);
        FntFlush(-1);
        display();
        fps_counter.measure_frames++;


    }

  return 0;
}


void display(void) {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    DrawOTag(&ot[db][OT_LEN - 1]);
    db = !db;
    nextpri = pribuff[db];
}

void init(void) {
    ResetGraph(0);
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, SCREEN_YRES);
    SetDefDispEnv(&disp[1], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[0], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, SCREEN_YRES);
    if (0)
    {
        SetVideoMode(MODE_PAL);
        disp[0].screen.y += 8;
        disp[1].screen.y += 8;
    }
    SetDispMask(1);                 // Display on screen
    setRGB0(&draw[0], 63, 0, 127);
    setRGB0(&draw[1], 63, 0, 127);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);
    FntOpen(MARGINX, SCREEN_YRES - MARGINY - FONTSIZE, SCREEN_XRES - MARGINX * 2, FONTSIZE, 0, 280 );


    GetTimInfo(ball_tile, &tim);

    LoadImage(tim.prect, tim.paddr);
    if (tim.mode & 0x8) {
        LoadImage(tim.crect, tim.caddr);
    }
}

void retrieve_tim() {
    printf("Upload texture...");

 }