/*
 * LibPSn00b Example Programs
 *
 * Off-screen Render to Texture Example
 * 2019 - 2021 Meido-Tek Productions / PSn00bSDK Project
 *
 * Demonstrates quick render to texture for multi-texture style effects,
 * view screens and more. This example also shows how to use multiple
 * ordering tables and chaining them together so it can all be rendered
 * with a single DrawOTag() call.
 *
 * Example by Lameguy64
 *
 * Changelog:
 *
 *	May 10, 2021		- Variable types updated for psxgpu.h changes.
 *
 *  Oct 26, 2019		- Initial version.
 *
 */

#include <inline_c.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <stdio.h>
#include <sys/types.h>

#define OTLEN                                                                  \
  8 // Ordering table length (recommended to set as a define
    // so it can be changed easily)

DISPENV disp[2]; // Display/drawing buffer parameters
DRAWENV draw[2];
int db = 0;

// PSn00bSDK requires having all u_long types replaced with
// u_int, as u_long in modern GCC that PSn00bSDK uses defines it as a 64-bit
// integer.

u_long ot[2][OTLEN];    // Ordering table length
char pribuff[2][32768]; // Primitive buffer
char *nextpri;          // Next primitive pointer

int tim_mode; // TIM image parameters
RECT tim_prect, tim_crect;
int tim_uoffs, tim_voffs;

void display() {

  DrawSync(0); // Wait for any graphics processing to finish

  VSync(0); // Wait for vertical retrace

  PutDispEnv(&disp[db]); // Apply the DISPENV/DRAWENVs
  PutDrawEnv(&draw[db]);

  SetDispMask(1); // Enable the display

  DrawOTag(ot[db] + OTLEN - 1); // Draw the ordering table

  db = !db; // Swap buffers on every pass (alternates between 1 and 0)
  nextpri = pribuff[db]; // Reset next primitive pointer
}

// Texture upload function
void LoadTexture(u_long *tim, TIM_IMAGE *tparam) {

  // Read TIM parameters (PSn00bSDK)
  GetTimInfo(tim, tparam);

  // Upload pixel data to framebuffer
  LoadImage(tparam->prect, (u_long *)tparam->paddr);
  DrawSync(0);

  // Upload CLUT to framebuffer if present
  if (tparam->mode & 0x8) {

    LoadImage(tparam->crect, (u_long *)tparam->caddr);
    DrawSync(0);
  }
}

void loadstuff(void) {

  TIM_IMAGE my_image; // TIM image parameters

  extern u_long tim_igu_tile[];

  // Load the texture
  LoadTexture(tim_igu_tile, &my_image);

  // Copy the TIM coordinates
  tim_prect = *my_image.prect;
  tim_crect = *my_image.crect;
  tim_mode = my_image.mode;

  // Calculate U,V offset for TIMs that are not page aligned
  tim_uoffs = (tim_prect.x % 64) << (2 - (tim_mode & 0x3));
  tim_voffs = (tim_prect.y & 0xff);
}

// To make main look tidy, init stuff has to be moved here
void init(void) {

  // Reset graphics
  ResetGraph(0);

  // First buffer
  SetDefDispEnv(&disp[0], 0, 0, 320, 240);
  SetDefDrawEnv(&draw[0], 0, 240, 320, 240);
  // Second buffer
  SetDefDispEnv(&disp[1], 0, 240, 320, 240);
  SetDefDrawEnv(&draw[1], 0, 0, 320, 240);

  draw[0].isbg = 1;              // Enable clear
  setRGB0(&draw[0], 63, 0, 127); // Set clear color (dark purple)
  draw[1].isbg = 1;
  setRGB0(&draw[1], 63, 0, 127);

  nextpri = pribuff[0]; // Set initial primitive pointer address

  // load textures and possibly other stuff
  loadstuff();

  // set tpage of lone texture as initial tpage
  draw[0].tpage = getTPage(tim_mode & 0x3, 0, tim_prect.x, tim_prect.y);
  draw[1].tpage = getTPage(tim_mode & 0x3, 0, tim_prect.x, tim_prect.y);

  // apply initial drawing environment
  PutDrawEnv(&draw[!db]);
}

int main() {

  TILE *tile; // Pointer for TILE
  SPRT *sprt; // Pointer for SPRT

  // Init stuff
  init();

  while (1) {

    ClearOTagR(ot[db], OTLEN); // Clear ordering table

    // Sort textured sprite

    sprt = (SPRT *)nextpri;

    setSprt(sprt);        // Initialize the primitive (very important)
    setXY0(sprt, 48, 48); // Position the sprite at (48,48)
    setWH(sprt, 64, 64);  // Set size to 64x64 pixels
    setUV0(sprt,          // Set UV coordinates
           tim_uoffs, tim_voffs);
    setClut(sprt, // Set CLUT coordinates to sprite
            tim_crect.x, tim_crect.y);
    setRGB0(sprt, // Set primitive color
            128, 128, 128);
    addPrim(ot[db], sprt); // Sort primitive to OT

    nextpri += sizeof(SPRT); // Advance next primitive address

    // Sort untextured tile primitive from the last tutorial

    tile = (TILE *)nextpri; // Cast next primitive

    setTile(tile);              // Initialize the primitive (very important)
    setXY0(tile, 32, 32);       // Set primitive (x,y) position
    setWH(tile, 64, 64);        // Set primitive size
    setRGB0(tile, 255, 255, 0); // Set color yellow
    addPrim(ot[db], tile);      // Add primitive to the ordering table

    nextpri += sizeof(TILE); // Advance the next primitive pointer

    // Update the display
    display();
  }

  return 0;
}