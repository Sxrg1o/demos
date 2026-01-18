#include <stdlib.h>

#include "systems.h"
#include "logic.h"

static bool is_denser(Cell* a, Cell* b) {
    if(b->state == SOLID) return false;
    return a->density > b->density;
}

static void swap_cells(Cell* a, Cell* b) {
    Cell temp = *a;
    *a = *b;
    *b = temp;
    a->life = b->life; 
}

void update_position(World* world) {
    world->global_clock = !world->global_clock;
    uint8_t clock = world->global_clock;

    for(int y=world->height-1; y>=0; y--) {
        for(int x=0; x<world->width; x++) {
            Cell* cell = get_cell(world, x, y);
            if(cell->is_empty || cell->state == SOLID || cell->life == clock) continue;
            cell->life = clock;

            int dir = (GetRandomValue(0, 1)==0) ? 1 : -1;

            if(cell->state == POWDER) {
                Cell* dest = get_cell(world, x, y+1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
                dest = get_cell(world, x+dir, y+1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
                dest = get_cell(world, x-dir, y+1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
            }
            else if(cell->state == LIQUID) {
                Cell* dest = get_cell(world, x, y+1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
                for(int i=0; i<=cell->dispersion; i++) {
                    int d = (i==0)? dir : dir*i; 
                    dest = get_cell(world, x+d, y);
                    if(dest && (dest->is_empty || is_denser(cell, dest))) {
                        swap_cells(cell, dest);
                        break; 
                    }
                }
            }
            else if(cell->state == GAS || cell->state == FIRE) {
                Cell* dest = get_cell(world, x, y-1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
                dest = get_cell(world, x+dir, y-1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
                dest = get_cell(world, x-dir, y-1);
                if(dest && (dest->is_empty || is_denser(cell, dest))) {
                    swap_cells(cell, dest);
                    continue;
                }
            }
        }
    }
}

void update_thermodynamics(World* world) {
    for(int i=0; i<world->width*world->height; i++) {
        Cell* cell = &world->cells[i];
        if(cell->is_empty || cell->state == FIRE) continue;

        MaterialFamily* fam = &world->families[cell->family_idx];

        if(cell->state == LIQUID && cell->temperature > fam->boiling_point) {
            cell->state = GAS;
            cell->density *= fam->density_gas_mult;
            cell->dispersion = 5;
            cell->base_color.a = 150; 
        }
        else if(cell->state == GAS && cell->temperature < fam->boiling_point) {
            cell->state = LIQUID;
            cell->density /= fam->density_gas_mult; 
            cell->dispersion = 2;
            cell->base_color.a = 255;
        }
        else if((cell->state == SOLID || cell->state == POWDER) && cell->temperature > fam->melting_point) {
            cell->state = LIQUID;
            cell->density *= fam->density_liq_mult;
            cell->dispersion = 1;
        }
        else if(cell->state == LIQUID && cell->temperature < fam->melting_point) {
            cell->state = SOLID;
            cell->density /= fam->density_liq_mult;
        }
        
        if((cell->chem_flags & FUEL) && cell->temperature > fam->ignite_temperature) {
            set_cell(cell, FIRE, ORANGE, -1.0f, 800.0f, FAM_DEFAULT, FUEL, 0.2f);
            cell->life = 100;
        }
    }
}

void update_chemistry(World* world) {
    for(int i=0; i<world->width*world->height; i++) {
        Cell* cell = &world->cells[i];
        if(cell->is_empty) continue;

        if(cell->state == FIRE || (cell->chem_flags & TOXIC)) {
            if(cell->life > 0) {
                cell->life--;
                if(cell->state == FIRE) {
                    cell->temperature = (float)(600 + GetRandomValue(0, 400));
                    cell->base_color.a = (uint8_t)((cell->life / 100.0f) * 255);
                }
            } else {
                if(cell->state == FIRE && GetRandomValue(0, 10)) {
                    set_cell(cell, GAS, DARKGRAY, 0.1f, cell->temperature, FAM_DEFAULT, TOXIC, 0.1f);
                    cell->life = 200; 
                } else {
                    set_cell_empty(cell);
                }
            }
        }
    }
}

void update_render(World* world) {
    ClearBackground(BLACK);
    for(int i=0; i<world->width*world->height; i++) {
        if(world->cells[i].is_empty) continue;
        int x = i % world->width;
        int y = i / world->width;
        DrawPixel(x, y, world->cells[i].base_color);
    }
}