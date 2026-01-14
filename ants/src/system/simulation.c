#include "simulation.h"
#include "../entities/ant.h"
#include "ant_world.h"
#include <stdlib.h>

// Entities Management
bool entity_spawn_ant(World *w, Position p) {
  if (!w || !world_in_bounds(w, p)) {
    return false;
  }

  if (w->num_ants >= MAX_ANTS) {
    return false;
  }

  if (world_is_occupied(w, p)) {
    return false;
  }

  int id = w->num_ants;
  Ant *ant = &w->ants[id];

  ant_init(ant, p, w->nest_pos);
  Cell cell;
  cell.type = CELL_ANT;
  cell.ant_id = id;
  cell.pheromone_food = 0.0f;
  cell.resource.type = RESOURCE_DIRT;
  cell.resource.value = 0;
  cell.resource.weight = 0;

  world_occupy_cell(w, p, cell);

  w->num_ants++;
  return true;
}

bool entity_kill_ant(int ant_id, World *w) {
  if (!w || ant_id < 0 || ant_id >= w->num_ants) {
    return false;
  }

  Ant *victim = &w->ants[ant_id];
  if (world_in_bounds(w, victim->position)) {
    Cell *c = &w->grid[victim->position.y * w->width + victim->position.x];
    if (c->type == CELL_ANT && c->ant_id == ant_id) {
      world_vacate_cell(w, victim->position);
    }
  }

  int last_id = w->num_ants - 1;
  if (ant_id != last_id) {
    Ant *last_ant = &w->ants[last_id];
    w->ants[ant_id] = *last_ant;

    Position p = last_ant->position;
    if (world_in_bounds(w, p)) {
      Cell *c = &w->grid[p.y * w->width + p.x];
      if (c->type == CELL_ANT && c->ant_id == last_id) {
        c->ant_id = ant_id;
      }
    }
  }

  w->num_ants--;
  return true;
}

bool entity_spawn_food(World *w, Position p, int radius) {
  if (!w)
    return false;

  bool placed = false;
  int r2 = radius * radius;

  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      if (dx * dx + dy * dy <= r2) {
        Position target = {p.x + dx, p.y + dy};

        if (world_in_bounds(w, target)) {
          Cell *c = &w->grid[target.y * w->width + target.x];

          c->type = CELL_RESOURCE;
          c->resource.type = RESOURCE_FOOD;
          c->resource.value = 10;
          c->resource.weight = 1;
          placed = true;
        }
      }
    }
  }
  return placed;
}

// Planification
void ant_think(int ant_id, World *w) {
  if (!w || ant_id < 0 || ant_id >= w->num_ants) {
    return;
  }

  Ant *ant = &(w->ants[ant_id]);
  ant_clear_plan(ant);
  int added = 0;
  for (int i = 0; i < PLAN_SIZE && added < PLAN_SIZE; i++) {
    Action a;
    a.type = ACTION_IDLE;
    a.target = ant->position;
    a.resource = (Resource){0};

    int choice = rand() % 4;

    if (choice == 1) {
      int attempts = 0;
      bool found = false;
      while (attempts < 16 && !found) {
        int dx = (rand() % (2 * ANT_DEFAULT_RADIUS + 1)) - ANT_DEFAULT_RADIUS;
        int dy = (rand() % (2 * ANT_DEFAULT_RADIUS + 1)) - ANT_DEFAULT_RADIUS;
        Position t = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, t) && !world_is_occupied(w, t)) {
          a.type = ACTION_MOVE;
          a.target = t;
          found = true;
        }
        attempts++;
      }
      if (!found) {
        a.type = ACTION_IDLE;
      }
    } else if (choice == 2) {
      Position candidates[256];
      int cand = 0;
      int r = ANT_DEFAULT_RADIUS;
      for (int dy = -r; dy <= r && cand < 256; dy++) {
        for (int dx = -r; dx <= r && cand < 256; dx++) {
          Position t = {ant->position.x + dx, ant->position.y + dy};
          if (!world_in_bounds(w, t))
            continue;
          Cell *c = &w->grid[t.y * w->width + t.x];
          if (c->type == CELL_RESOURCE) {
            candidates[cand++] = t;
          }
        }
      }
      if (cand > 0 && !ant->is_carring) {
        Position t = candidates[rand() % cand];
        a.type = ACTION_PICKUP;
        a.target = t;
      } else {
        a.type = ACTION_IDLE;
      }
    } else if (choice == 3) {
      if (ant->is_carring) {
        int attempts = 0;
        bool found = false;
        while (attempts < 16 && !found) {
          int dx = (rand() % (2 * ANT_DEFAULT_RADIUS + 1)) - ANT_DEFAULT_RADIUS;
          int dy = (rand() % (2 * ANT_DEFAULT_RADIUS + 1)) - ANT_DEFAULT_RADIUS;
          Position t = {ant->position.x + dx, ant->position.y + dy};
          if (world_in_bounds(w, t)) {
            Cell *c = &w->grid[t.y * w->width + t.x];
            if (c->type == CELL_EMPTY) {
              a.type = ACTION_DROP;
              a.target = t;
              a.resource = ant->carried_resource;
              found = true;
            }
          }
          attempts++;
        }
        if (!found)
          a.type = ACTION_IDLE;
      } else {
        a.type = ACTION_IDLE;
      }
    }

    ant->plan[added++] = a;
    ant->plan_length = added;
  }
}

// Simulation
void system_update_ant(int ant_id, World *world) {
  if (!world || ant_id < 0 || ant_id >= world->num_ants) {
    return;
  }

  Ant *ant = &(world->ants[ant_id]);

  Action ant_action;
  if (!ant_next_action(ant, &ant_action)) {
    ant_think(ant_id, world);
    return;
  }

  switch (ant_action.type) {
  case ACTION_IDLE:
    break;

  case ACTION_MOVE: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target) || world_is_occupied(world, target)) {
      ant_clear_plan(ant);
      return;
    }

    /* vacate old cell */
    if (world_in_bounds(world, ant->position)) {
      world_vacate_cell(world, ant->position);
    }

    /* occupy new cell as ant */
    Cell new_cell;
    new_cell.type = CELL_ANT;
    new_cell.ant_id = (ant_id >= 0) ? ant_id : 0;
    new_cell.pheromone_food = 0.0f;
    new_cell.resource = (Resource){0};
    world_occupy_cell(world, target, new_cell);

    ant->position = target;
    break;
  }

  case ACTION_PICKUP: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target) || ant->is_carring) {
      ant_clear_plan(ant);
      return;
    }

    Cell *c = &world->grid[target.y * world->width + target.x];
    if (c->type != CELL_RESOURCE) {
      ant_clear_plan(ant);
      return;
    }

    /* pick up the resource */
    ant->is_carring = true;
    ant->carried_resource = c->resource;

    /* remove resource from world */
    c->type = CELL_EMPTY;
    c->resource = (Resource){0};
    break;
  }

  case ACTION_DROP: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target) || !ant->is_carring) {
      ant_clear_plan(ant);
      return;
    }

    Cell *c = &world->grid[target.y * world->width + target.x];
    if (c->type != CELL_EMPTY) {
      ant_clear_plan(ant);
      return;
    }

    Cell drop;
    drop.type = CELL_RESOURCE;
    drop.resource = ant->carried_resource;
    drop.pheromone_food = 0.0f;
    drop.ant_id = 0;
    world_occupy_cell(world, target, drop);

    ant->is_carring = false;
    ant->carried_resource = (Resource){0};
    break;
  }
  }
}

// Update helpers
void update_world(World *w) {
  if (!w)
    return;
  for (int i = 0; i < w->num_ants; i++) {
    system_update_ant(i, w);
  }
}