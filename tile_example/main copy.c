#include <psxetc.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define OTLEN 8 // Ordering Table length
#define true 1
#define false 0
#define SCREEN_XRES 320
#define SCREEN_YRES 240
#define TILE_SIZE 8
DISPENV disp[2];
DRAWENV draw[2];
int db;

u_long ot[2][OTLEN];
char pribuff[2][1024 * 64]; // 32 KiloBytes primitive Buffer
char *nextpri;

struct Paddle {
  TILE *tile;
  int x;
  int y;
  int weight;
  int height;
};

void init(void) {

  TILE *tile;
  ResetGraph(0);

  // First buffer
  SetDefDispEnv(&disp[0], 0, 0, 320, 240);
  SetDefDrawEnv(&draw[0], 0, 240, 320, 240);
  // Second buffer
  SetDefDispEnv(&disp[1], 0, 240, 320, 240);
  SetDefDrawEnv(&draw[1], 0, 0, 320, 240);

  draw[0].isbg = 1;
  setRGB0(&draw[0], 63, 0, 127);
  draw[1].isbg = 1;
  setRGB0(&draw[1], 63, 0, 127);

  PutDispEnv(&disp[0]);
  PutDispEnv(&disp[1]);

  db = 0;
  nextpri = pribuff[0]; // Set initial primitive pointer address

  FntLoad(960, 256);
  FntOpen(0, 8, 320, 224, 0, 100);
}

void display(void) {
  DrawSync(0);
  VSync(0);

  PutDispEnv(&disp[db]);
  PutDrawEnv(&draw[db]);

  SetDispMask(1);

  DrawOTag(ot[db] + OTLEN - 1);
  db = !db;

  nextpri = pribuff[db];
}

void setPositionTile(TILE *tile, int x, int y) { setXY0(tile, x, y); };

int main() {
  TILE *tile;
  TILE *p1;

  init();
  int x = SCREEN_XRES / 2, y = SCREEN_YRES / 2;
  int x2 = 160, y2 = 160;
  int sumCollide = 0;
  int isCollide = false;
  int dx = 1, dy = 1;
  int randNumber = 0;

  while (1) {
    ClearOTagR(ot[db], OTLEN);

    if (x < 0 || x > (SCREEN_XRES - TILE_SIZE)) {
      dx = -dx - 1;
      randNumber = rand() % 255;
    }

    if (y < 0 || y > (SCREEN_YRES - TILE_SIZE)) {
      dy = -dy - 1;
      randNumber = rand() % 255;
    }

    x += dx;
    y += dy;

    if ((tile->w <= p1->x0 + p1->w) && (tile->x0 + tile->w >= p1->x0) &&
        (tile->y0 <= p1->y0 + p1->h) && (tile->y0 + tile->h >= p1->y0)) {
      isCollide = true;

    } else {
      isCollide = false;
    }

    if (x < p1->x0 || x > (p1->w - TILE_SIZE)) {
      printf("CONDITION VERDADEIRA\n");
    }

    tile = (TILE *)nextpri;

    FntPrint(-1, "Colidiram? %d \n", isCollide);
    FntPrint(-1, "TILE1 x0 = %d y0 = %d w = %d h = %d\n", tile->x0, tile->y0,
             tile->w, tile->h);
    FntPrint(-1, "p1 x0 = %d y0 = %d w = %d h = %d\n", p1->x0, p1->y0, p1->w,
             p1->h);

    FntFlush(-1);
    setTile(tile);
    setTile(p1);
    setPositionTile(tile, x, y);
    setWH(tile, TILE_SIZE, TILE_SIZE);
    setPositionTile(p1, 10, 100);
    setWH(p1, 8, 80);
    setRGB0(tile, randNumber, randNumber - 100, 0);

    printf("%d ", db);
    addPrim(ot[db], tile); // Add primitive to the ordering table
    addPrim(ot[db], p1);   // Add primitive to the ordering table

    nextpri += sizeof(TILE);

    display();
  }
  return 0;
}
