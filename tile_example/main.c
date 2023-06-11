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
#define TILE_SIZE 64
DISPENV disp[2];
DRAWENV draw[2];
int db;

u_long ot[2][OTLEN];
char pribuff[2][1024 * 32]; // 32 KiloBytes primitive Buffer
char *nextpri;              // Next Primitive pointer

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

int calculateDistance(int x1, int y1, int x2, int y2) {
  int result = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
  return result * 0.5; // Sqrt using multiply
};
int main() {
  TILE *tile;
  TILE *tile2;

  init();
  int x = 0, y = 0;
  int x2 = 160, y2 = 160;
  int distance = 0;
  int isCollide = false;
  int dx = 1, dy = 1;
  int randNumber = 0;

  while (1) {
    ClearOTagR(ot[db], OTLEN);

    if (x < 0 || x > (SCREEN_XRES - TILE_SIZE)) {
      dx = -dx;
      randNumber = rand() % 255;
    }

    if (y < 0 || y > (SCREEN_YRES - TILE_SIZE)) {
      dy = -dy;
      randNumber = rand() % 255;
    }

    x += dx;
    y += dy;

    if ((tile->w <= tile2->x0 + tile2->w) &&
        (tile->x0 + tile->w >= tile2->x0) &&
        (tile->y0 <= tile2->y0 + tile2->h) &&
        (tile->y0 + tile->h >= tile2->y0)) {
      isCollide = true;

      tile->x0 = tile->x0 - dx;
      tile->y0 = tile->y0 - dy;
    } else {
      isCollide = false;
    }

    distance = calculateDistance(x, y, x2, y2);

    tile = (TILE *)nextpri;

    FntPrint(-1, "Hello World!  GAME SOFT\n");
    FntPrint(-1, "Distancia entre TILE1 e TILE2 = %d \n", distance);
    FntPrint(-1, "Colidiram? %d \n", isCollide);

    FntFlush(-1);
    setTile(tile);
    setTile(tile2);
    setPositionTile(tile, x, y);
    setWH(tile, TILE_SIZE, TILE_SIZE);
    setPositionTile(tile2, 160, 160);
    setWH(tile2, TILE_SIZE, TILE_SIZE);
    setRGB0(tile, randNumber, randNumber - 100, 0);

    addPrim(ot[db], tile);  // Add primitive to the ordering table
    addPrim(ot[db], tile2); // Add primitive to the ordering table

    printf("TILE1 x0 = %d y0 = %d w = %d h = %d\n", tile->x0, tile->y0, tile->w,
           tile->h);
    printf("TILE2 x0 = %d y0 = %d w = %d h = %d\n", tile2->x0, tile2->y0,
           tile2->w, tile2->h);

    nextpri += sizeof(TILE);
    display();
  }
  return 0;
}
