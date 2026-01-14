#ifndef RENDER_H
#define RENDER_H

#include "../entities/ant.h"
#include "../entities/resource.h"
#include "ant_world.h"
#include <stdint.h>

#define COLOR_SKY 0x87CEEBFF   // Light Blue (R=87, G=CE, B=EB)
#define COLOR_DIRT 0x8B4513FF  // Saddle Brown (R=8B, G=45, B=13)
#define COLOR_FOOD 0xFF0000FF  // Red
#define COLOR_ANT 0x000000FF   // Black
#define COLOR_NEST 0xFFD700FF  // Gold
#define COLOR_OTHER 0x808080FF // Grey
#define CELL_SIZE 4

void render_world(const World *w);

#endif // RENDER_H
