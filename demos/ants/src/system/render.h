#ifndef RENDER_H
#define RENDER_H

#include "../entities/ant.h"
#include "../entities/resource.h"
#include "ant_world.h"
#include <stdint.h>

// Colors (0xAABBGGRR for Little Endian RGBA8888)
#define COLOR_SKY       0xFFEBCE87 // Light Blue (R=87, G=CE, B=EB)
#define COLOR_EARTH     0xFF13458B // Saddle Brown (R=8B, G=45, B=13)
#define COLOR_FOOD      0xFF0000FF // Red
#define COLOR_STRUCTURE 0xFF58DBFF // Mustard (R=FF, G=DB, B=58)
#define COLOR_ANT       0xFF000000 // Black
#define COLOR_OTHER     0xFF808080 // Grey

// Renders the simulation state into a pixel buffer.
// pixels: Array of size world->width * world->height.
// width/height: dimensions of the pixel buffer (should match world).
void render_scene(const World *world, 
                  const Ant *ants, int ant_count, 
                  const Resource *resources, int resource_count, 
                  uint32_t *pixels, int width, int height);

#endif // RENDER_H
