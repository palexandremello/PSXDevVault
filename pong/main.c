
#include <sys/types.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <psxetc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include <psxspu.h>
#include <psxapi.h>
#include <inline_c.h>
#include <hwregs_c.h>
#include <string.h>
#include "controller.h"
#include "initialize.h"
#include "coords.h"
#include "ball.h"
#include "collision_system.h"


#define OT_LEN 8
#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define MARGINX 32      // margins for text display
#define MARGINY 44
#define FONTSIZE 8 * 3          // Text Field Height
#define CENTER_X SCREEN_XRES / 2
#define CENTER_Y SCREEN_YRES / 2
#define GAME_DURATION 10
#define ALLOC_START_ADDR 0x1010

extern const uint8_t ball_effect[];

static int next_channel     = 0;
static int next_sample_addr = ALLOC_START_ADDR;

typedef struct {
	uint32_t magic;			// 0x70474156 ("VAGp") for mono files
	uint32_t version;
	uint32_t interleave;	// Unused in mono files
	uint32_t size;			// Big-endian, in bytes
	uint32_t sample_rate;	// Big-endian, in Hertz
	uint16_t _reserved[5];
	uint16_t channels;		// Unused in mono files
	char     name[16];
} VAG_Header;

#define SWAP_ENDIAN(x) ( \
	(((uint32_t) (x) & 0x000000ff) << 24) | \
	(((uint32_t) (x) & 0x0000ff00) <<  8) | \
	(((uint32_t) (x) & 0x00ff0000) >>  8) | \
	(((uint32_t) (x) & 0xff000000) >> 24) \
)


DISPENV disp[2];
DRAWENV draw[2];
int db = 0;
int factor = 1;

uint32_t ot[2][OT_LEN];
char pribuff[2][16384];
char *nextpri;

char buffer[2][34];
extern const uint32_t  ball_tile[];

TIM_IMAGE tim;

int score_player1,score_player2 = 0;
int aiSpeed = 5;

void init(void);
void display();
void setHalfHeight(Paddle* paddle);
void setHalfWidth(Paddle* paddle);
void retrieve_tim();
void resetBall(Coords *coords, Ball *ball, int whoLost);
void updateAi(Paddle *player, Coords *coords);
void play_sample(int addr, int sample_rate);
int upload_sample(const void *data, int size);



int main(int argc, const char* argv[]) {
    init();
    SpuInit();

    SPRT_16 *sprt;
    DR_TPAGE *tpri;
    TILE *tile, *tile2;
    Paddle player1, player2;
    PADTYPE  *controller1, *controller2;
    struct Coords coords;
    struct Ball ball;

    VAG_Header *ballfx_vag = (VAG_Header *) ball_effect;

    int ballfx_addr = upload_sample(&ballfx_vag[1],
                                    SWAP_ENDIAN(ballfx_vag->size));
    int ballfx_sr   = SWAP_ENDIAN(ballfx_vag->sample_rate);

    Color color;

    player1.h = 60;
    player1.w = 12;
    player1.y = CENTER_Y - 42;
    player1.x = 10;


    player2.h = 60;
    player2.w = 12;
    player2.y = CENTER_Y - 42;
    player2.x = SCREEN_XRES - 20;

    setHalfHeight(&player1);
    setHalfWidth(&player1);
    setHalfHeight(&player2);
    setHalfWidth(&player2);

    srand(64);
    color.red = (rand() % 256);
    color.green = (rand() % 256);
    color.blue = (rand() %  256);

    ball.w = 16;
    ball.h = 16;
    ball.x = 1 - (rand() % 3);
    ball.y = 1 - (rand() % 3);
    ball.radius = 16;
    if (!ball.x) {
        ball.x = 1;
    }
    if (!ball.y) {
        ball.y = 1;
    }

    ball.x *= 2;
    ball.y *= 2;

    ball.color = color;

    coords.x = CENTER_X - ball.w / 2;
    coords.y = CENTER_Y - ball.h / 2;
    coords.dx = 3;
    coords.dy = 3;

    initControllers();
    while (1) {
        ClearOTagR(ot[db], OT_LEN);
        FntPrint(-1, "Player 1: %d - Player2: %d\n", score_player1, score_player2);
        FntFlush(-1);
        controller1 = (PADTYPE *)&buffer[0][0];
        controller2 = (PADTYPE *)&buffer[1][0];
        coords.x += coords.dx;
        coords.y += coords.dy;


        if (coords.y < 0 || coords.y > (SCREEN_YRES - (16 / 2))) {

            coords.dy = -coords.dy;
        }



//        FntPrint(-1, "FPS: %d\n", fps_counter.value);
        handlerControllerInput(controller1, &player1);
        handlerControllerInput(controller2, &player2);

        if (player1.y < 0) player1.y = 0;
        if(player1.y  > (SCREEN_YRES - player1.h)) player1.y = SCREEN_YRES - player1.h;

        if (player2.y < 0) player2.y = 0;
        if (player2.y > SCREEN_YRES - player2.h) player2.y = SCREEN_YRES - player2.h;

        updateAi(&player2, &coords);
        updateAi(&player1, &coords);

        if (detectCollision(&player1, &coords, &ball))  {
            handleCollision(&player1, &coords, &ball, factor);
            play_sample(ballfx_addr, ballfx_sr);
        }

        if (detectCollision(&player2, &coords, &ball))  {
            handleCollision(&player2, &coords, &ball, factor);
            play_sample(ballfx_addr, ballfx_sr);

        }

        if (coords.x < 0) {
            score_player2++;
            resetBall(&coords, &ball, 1);
        } else if (coords.x > (SCREEN_XRES - 16)) {
            score_player1++;
            resetBall(&coords, &ball, 2);

        }


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

        display();
        fps_counter.measure_frames++;




    }

  return 0;
}

int upload_sample(const void *data, int size) {
	int _addr = next_sample_addr;
	int _size = (size + 63) & 0xffffffc0;

	SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
	SpuSetTransferStartAddr(_addr);

	SpuWrite((const uint32_t *) data, _size);
	SpuIsTransferCompleted(SPU_TRANSFER_WAIT);

	next_sample_addr = _addr + _size;
	return _addr;
}


void play_sample(int addr, int sample_rate) {
	int ch = next_channel;

	// Make sure the channel is stopped.
	SpuSetKey(0, 1 << ch);

	SPU_CH_FREQ(ch) = getSPUSampleRate(sample_rate);
	SPU_CH_ADDR(ch) = getSPUAddr(addr);

	SPU_CH_VOL_L(ch) = 0x3fff;
	SPU_CH_VOL_R(ch) = 0x3fff;
	SPU_CH_ADSR1(ch) = 0x00ff;
	SPU_CH_ADSR2(ch) = 0x0000;

	SpuSetKey(1, 1 << ch);

	next_channel = (ch + 1) % 24;
}

void display(void) {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    DrawOTag(&ot[db][OT_LEN - 1]);
    SetDispMask(1);                 // Display on screen

    db = !db;
    nextpri = pribuff[db];
}

void init(void) {
    ResetGraph(0);
    //SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, SCREEN_YRES);
    //SetDefDispEnv(&disp[1], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    //SetDefDrawEnv(&draw[0], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
    //SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, SCREEN_YRES);

    if( GetVideoMode() == MODE_NTSC ) {
        SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, SCREEN_YRES);
        SetDefDispEnv(&disp[1], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
        SetDefDrawEnv(&draw[0], 0, SCREEN_YRES, SCREEN_XRES, SCREEN_YRES);
        SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, SCREEN_YRES);
        printf("NTSC System.\n");
    } else {
        SetDefDispEnv(&disp[0], 0, 0, SCREEN_XRES, 256);
        SetDefDispEnv(&disp[1], 0, 256, SCREEN_XRES, 256);
        SetDefDrawEnv(&draw[0], 0, 256, SCREEN_XRES, 256);
        SetDefDrawEnv(&draw[1], 0, 0, SCREEN_XRES, 256);
        printf("PAL System.\n");
    }

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

void resetBall(Coords *coords, Ball *ball, int whoLost) {
    coords->x = CENTER_X - ball->w / 2;
    coords->y = CENTER_Y - ball->h / 2;

    int dir = (whoLost == 1) ? -1 : 1;
    coords->dx = dir * (2 + rand() % 2);
    coords->dy = 2 + rand() % 2;
    for (int i = 0; i < 1; i++) VSync(0);

}

void updateAi(Paddle *player, Coords *coords) {
    if (coords->y < player->y + (player->h /2)) {
        player->y -= aiSpeed;
        if(player->y < 0) player->y = 0;
    }

    else if (coords->y > player->y  + (player->h / 2)) {
        player->y += aiSpeed;
        if (player->y > SCREEN_YRES - player->h) player->y = SCREEN_YRES - player->h;
    }
}

void retrieve_tim() {
    printf("Upload texture...\n");
    GetTimInfo(ball_tile, &tim);

    LoadImage(tim.prect, tim.paddr);
    if (tim.mode & 0x8) {
        LoadImage(tim.crect, tim.caddr);
    }
 }