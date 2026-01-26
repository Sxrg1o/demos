#include "ant_render.h"
#include <math.h>
#include <raylib.h>

// Helper to apply noise/variant to a color
static Color apply_variant(Color base, uint8_t variant) {
  // Map variant (0-255) to factor (0.85 - 1.15)
  float factor = 0.85f + (variant / 255.0f) * 0.30f;
  
  return (Color){
      (unsigned char)fminf(base.r * factor, 255.0f),
      (unsigned char)fminf(base.g * factor, 255.0f),
      (unsigned char)fminf(base.b * factor, 255.0f), 
      base.a
  };
}

void render_world(const World *w) {
  ClearBackground(GetColor(COLOR_BACKGROUND));

  for (int x = 0; x < w->width; x++) {
    for (int y = 0; y < w->height; y++) {
      Cell cell = w->grid[y * w->width + x];
      Color cell_color;
      bool use_variant = false;
      uint8_t variant = 0;

      // Determine Base Color and Variant
      switch (cell.type) {
        case CELL_EMPTY:
          cell_color = GetColor(COLOR_BACKGROUND);
          use_variant = true;
          variant = cell.bg_variant;
          break;

        case CELL_ANT:
          cell_color = GetColor(COLOR_ANT);
          // Ants might no need texture noise, or we could use ID hash
          break;

        case CELL_NEST:
          cell_color = GetColor(COLOR_NEST);
          break;

        case CELL_RESOURCE:
          use_variant = true;
          variant = cell.resource.variant;
          switch (cell.resource.type) {
            case RESOURCE_DIRT:
              cell_color = GetColor(COLOR_DIRT);
              break;
            case RESOURCE_FOOD:
              cell_color = GetColor(COLOR_FOOD);
              break;
            case RESOURCE_RESERVOIR:
              cell_color = GetColor(COLOR_RESERVOIR);
              break;
            default:
              cell_color = GetColor(COLOR_OTHER);
              break;
          }
          break;

        case CELL_OUT_OF_BOUNDS:
          cell_color = DARKGRAY;
          break;
          
        default:
          cell_color = PINK;
          break;
      }

      // Apply Noise
      if (use_variant) {
        cell_color = apply_variant(cell_color, variant);
      }

      // Apply Pheromones (Visual Overlay)
      float p_food = cell.pheromone_food;
      float p_build = cell.pheromone_build;
      float max_p = fmaxf(p_food, p_build);
      
      // Optimization: Only process pheromones if visible
      if (max_p > 0.01f) {
        // Clamp 0-1
        p_food = fminf(p_food, 1.0f);
        p_build = fminf(p_build, 1.0f);

        // Pheromone Color Mixing
        // Food = Red component, Build = Blue component
        // We add this tint to the cell color
        
        // Let's use alpha blending for pheromones
        // Color pher = {255 (if food), 0, 255 (if build), alpha based on intensity}
        
        // Simple visualization:
        // Tint towards Green for food path, Blue for build path?
        // Original code used R for food, B for build.
        
        // Red (Food)
        if (p_food > 0.01f) {
            cell_color.r = (unsigned char)fminf(cell_color.r + p_food * 100, 255);
        }
        // Blue (Build)
        if (p_build > 0.01f) {
            cell_color.b = (unsigned char)fminf(cell_color.b + p_build * 100, 255);
        }
      }

      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, cell_color);
    }
  }
}
