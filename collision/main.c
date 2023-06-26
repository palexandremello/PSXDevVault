
#include <stdio.h>
#include <stdlib.h>
#include <psxgpu.h>
#include "controller.h"
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
void setHalfHeight(Paddle* paddle);
void setHalfWidth(Paddle* paddle);
void retrieve_tim();

void rebootMe() {
    ((void *(*)())0xbfc00000)();
};

char buffer[2][34];
extern const uint32_t  ball_tile[];

TIM_IMAGE tim;

int main() {
    SPRT_16 *sprt;
    DR_TPAGE *tpri;
    TILE *tile, *tile2;
    Paddle player1, player2;
    PADTYPE  *controller1;
    struct Coords coords;
    struct Ball ball;
    Color color;

    // Paddle informations

    player1.h = 100;
    player1.w = 12;
    player1.y = 80;
    player1.x = 10;


    player2.h = 100;
    player2.w = 12;
    player2.y = 80;
    player2.x = SCREEN_XRES - 20;

    setHalfHeight(&player1);
    setHalfWidth(&player1);
    setHalfHeight(&player2);
    setHalfWidth(&player2);

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

    int x = 0, y = 0;
    init();
    initControllers();
    VSyncCallback(callback_vsync);
    while (1) {
        ClearOTagR(ot[db], OT_LEN);

        controller1 = (PADTYPE *)&buffer[0][0];
        coords.x += coords.dx;
        coords.y += coords.dy;


        if (coords.x < 0 || coords.x > (SCREEN_XRES - 16)) {
            coords.dx = -coords.dx;
        }

        if (coords.y < 0 || coords.y > (SCREEN_YRES - 16)) {
            coords.dy = -coords.dy;
        }

        FntPrint(-1, "FPS: %d\n", fps_counter.value);
        FntPrint(-1, "PADDLE 1 (w, h) = (%d, %d)\n", player1.w, player1.h);
        FntPrint(-1, "PADDLE 1 (hw, hh) (%d, %d)\n", player1.hw, player1.hh);

        handlerControllerInput(controller1, &player1);
        sprt = (SPRT_16 *)nextpri;

        setSprt16(sprt);
        setXY0(sprt, coords.x, coords.y);
        setRGB0(sprt, ball.color.red, ball.color.green, ball.color.blue);
        setUV0(sprt, 0, 0);
        setClut(sprt, tim.crect->x, tim.crect->y);
        addPrim(ot[db] + (OT_LEN -1), sprt);

        nextpri += sizeof(SPRT_16);
        tpri = (DR_TPAGE *)nextpri;
        setDrawTPage(tpri, 0, 0, getTPage(0, 0, tim.prect->x, tim.prect->y));
        addPrim(ot[db] + (OT_LEN -1), tpri);
        nextpri += sizeof(DR_TPAGE);

        tile = (TILE*)nextpri;

        setTile(tile);
        setXY0(tile, player1.x, player1.y);
        setWH(tile, player1.w, player1.h);
        setRGB0(tile, 255, 255, 0);

        addPrim(ot[db] + (OT_LEN -1), tile);

        nextpri += sizeof(TILE);

        tile2 = (TILE*)nextpri;

        setTile(tile2);
        setXY0(tile2, player2.x, player2.y);
        setWH(tile2, player2.w, player2.h);
        setRGB0(tile2, 255, 255, 0);

        addPrim(ot[db] + (OT_LEN -1), tile2);

        nextpri += sizeof(TILE);

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
    SetDispMask(1);                 // Display on screen
    setRGB0(&draw[0], 63, 0, 127);
    setRGB0(&draw[1], 63, 0, 127);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    InitPAD(&buffer[0][0], 34, &buffer[1][0], 34);

    StartPAD();

    ChangeClearPAD(0);

    FntLoad(960, 0);
    FntOpen(MARGINX, SCREEN_YRES - MARGINY - FONTSIZE, SCREEN_XRES - MARGINX * 2, FONTSIZE, 0, 280 );
    retrieve_tim();
}

void setHalfWidth(Paddle* paddle) {

    paddle->hw = paddle->w  / 2;
}

void setHalfHeight(Paddle* paddle) {

    paddle->hh = paddle->h  / 2;
}


void retrieve_tim() {
    printf("Upload texture...\n");
    GetTimInfo(ball_tile, &tim);

    LoadImage(tim.prect, tim.paddr);
    if (tim.mode & 0x8) {
        LoadImage(tim.crect, tim.caddr);
    }
 }