#include "simulation.h"
#include "../entities/ant.h"
#include "ant_math.h"
#include "ant_world.h"
#include <math.h>
#include <stdlib.h>

// Entities Management
bool entity_spawn_ant(World *w, Position p) {
  if (!w || !world_in_bounds(w, p)) {
    return false;
  }

  if (w->nest.num_ants >= MAX_ANTS) {
    return false;
  }

  if (world_is_occupied(w, p)) {
    return false;
  }

  int id = w->nest.num_ants;
  Ant *ant = &w->nest.ants[id];
  ant_init(ant, id, p, w->nest.position);
  Cell cell = w->grid[p.y * w->width + p.x];
  cell.type = CELL_ANT;
  cell.ant_id = id;

  world_occupy_cell(w, p, cell);

  w->nest.num_ants++;
  return true;
}

bool entity_kill_ant(int ant_id, World *w) {
  if (!w || ant_id < 0 || ant_id >= w->nest.num_ants) {
    return false;
  }

  Ant *victim = &w->nest.ants[ant_id];
  if (world_in_bounds(w, victim->position)) {
    Cell *c = &w->grid[victim->position.y * w->width + victim->position.x];
    if (c->type == CELL_ANT && c->ant_id == ant_id) {
      world_vacate_cell(w, victim->position);
    }
  }

  int last_id = w->nest.num_ants - 1;
  if (ant_id != last_id) {
    Ant *last_ant = &w->nest.ants[last_id];
    w->nest.ants[ant_id] = *last_ant;

    /* ensure moved ant has correct id */
    w->nest.ants[ant_id].id = ant_id;

    Position p = last_ant->position;
    if (world_in_bounds(w, p)) {
      Cell *c = &w->grid[p.y * w->width + p.x];
      if (c->type == CELL_ANT && c->ant_id == last_id) {
        c->ant_id = ant_id;
      }
    }
  }

  w->nest.num_ants--;
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
  if (!w || ant_id < 0 || ant_id >= w->nest.num_ants) {
    return;
  }

  Ant *ant = &w->nest.ants[ant_id];
  ant_clear_plan(ant);

  int obs_radius = ANT_RADIUS;
  int obs_radius_sq = obs_radius * obs_radius;

  switch (ant->state) {
  case STATE_SCOUTING: {
    // 0. Check for food in observation radius
    bool food_found = false;
    int min_dist_f = 10000;

    for (int dy = -obs_radius; dy <= obs_radius; dy++) {
      for (int dx = -obs_radius; dx <= obs_radius; dx++) {
        if (dx * dx + dy * dy > obs_radius_sq)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
            float d = dist_sq(ant->position, p);
            if (d < min_dist_f) {
              min_dist_f = d;
              food_found = true;
            }
          }
        }
      }
    }

    if (food_found) {
      ant->state = STATE_HARVESTING;
      // We don't plan yet, next tick will handle HARVESTING logic towards this
      // food or we can just fall through or return. Let's return to let state
      // machine cycle.
      return;
    }

    // 1. Movement: Desire = Repulsion(Nest) + Attraction(Unknown)
    AntVector v_nest =
        AntVector_from_positions(w->nest.position, ant->position);
    v_nest = AntVector_normalize(v_nest);

    AntVector v_unknown = {0, 0};
    float min_visited = 100000.0f;
    Position best_p = ant->position;

    // Scan neighbors for least visited
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->pheromone_visited < min_visited) {
            min_visited = c->pheromone_visited;
            best_p = p;
          }
        }
      }
    }

    if (!Position_equals(best_p, ant->position)) {
      v_unknown = AntVector_from_positions(ant->position, best_p);
      v_unknown = AntVector_normalize(v_unknown);
    }

    AntVector v_desire = AntVector_add(AntVector_scale(v_nest, 0.4f),
                                       AntVector_scale(v_unknown, 0.6f));
    v_desire = AntVector_normalize(v_desire);

    // Direction to Target
    int tx =
        ant->position.x + (int)(v_desire.x + (v_desire.x > 0 ? 0.5f : -0.5f));
    int ty =
        ant->position.y + (int)(v_desire.y + (v_desire.y > 0 ? 0.5f : -0.5f));
    Position target = {tx, ty};

    // Case C: Random Initiative (Excavation)
    if ((rand() % 100) < 5) {
      int rx = (rand() % 3) - 1;
      int ry = (rand() % 3) - 1;
      Position rp = {ant->position.x + rx, ant->position.y + ry};
      if (world_in_bounds(w, rp)) {
        Cell *rc = &w->grid[rp.y * w->width + rp.x];
        if (rc->type == CELL_RESOURCE && rc->resource.type == RESOURCE_DIRT) {
          target = rp; // Override target to excavate
        }
      }
    }

    if (world_in_bounds(w, target)) {
      Cell *tc = &w->grid[target.y * w->width + target.x];

      if (tc->type == CELL_EMPTY) {
        ant->plan[0] = (Action){ACTION_MOVE, target, {0}};
        ant->plan_length = 1;
      } else if (tc->type == CELL_RESOURCE &&
                 tc->resource.type == RESOURCE_DIRT) {
        // Case B: Blocked -> Excavate?
        if ((rand() % 100) < 80) {
          ant->plan[0] = (Action){ACTION_PICKUP, target, {0}};
          ant->plan_length = 1;
          ant->state = STATE_CLEARING;
        }
      }
    }
    // Fallback: Random move if stuck or null vector
    if (ant->plan_length == 0) {
      int rx = (rand() % 3) - 1;
      int ry = (rand() % 3) - 1;
      Position rp = {ant->position.x + rx, ant->position.y + ry};
      if (world_in_bounds(w, rp)) {
        Cell *rc = &w->grid[rp.y * w->width + rp.x];
        if (rc->type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, rp, {0}};
          ant->plan_length = 1;
        }
      }
    }
    break;
  }

  case STATE_HARVESTING: {
    // 1. Check direct vision of food
    Position target_food = {-1, -1};

    // First check immediate neighbors for quick grab
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
            ant->plan[0] = (Action){ACTION_PICKUP, p, {0}};
            ant->plan_length = 1;
            ant->state = STATE_RETURNING;
            return;
          }
        }
      }
    }

    // Check observation radius for food to go to
    for (int dy = -obs_radius; dy <= obs_radius; dy++) {
      for (int dx = -obs_radius; dx <= obs_radius; dx++) {
        if (dx * dx + dy * dy > obs_radius_sq)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
            target_food = p;
            goto found_food;
          }
        }
      }
    }

  found_food:;

    // If we see food, move towards it.
    if (world_in_bounds(w, target_food)) {
      AntVector v = AntVector_from_positions(ant->position, target_food);
      v = AntVector_normalize(v);
      int tx = ant->position.x + (int)(v.x + (v.x > 0 ? 0.5f : -0.5f));
      int ty = ant->position.y + (int)(v.y + (v.y > 0 ? 0.5f : -0.5f));
      Position next_step = {tx, ty};

      if (world_in_bounds(w, next_step)) {
        Cell *nc = &w->grid[next_step.y * w->width + next_step.x];
        // Obstacle Rule: If DIRT -> Excavate
        if (nc->type == CELL_RESOURCE && nc->resource.type == RESOURCE_DIRT) {
          ant->plan[0] = (Action){ACTION_PICKUP, next_step, {0}};
          ant->plan_length = 1;
          ant->state = STATE_CLEARING; // Taking dirt to clear path
          return;
        } else if (nc->type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, next_step, {0}};
          ant->plan_length = 1;
          return;
        }
      }
    }

    // If no food in sight (or blocked by non-dirt), follow pheromones or wander
    // Follow Food Pheromone
    Position best_phero_pos = {-1, -1};
    float max_pher = -1.0f;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->pheromone_to_food > max_pher) {
            max_pher = c->pheromone_to_food;
            best_phero_pos = p;
          }
        }
      }
    }

    if (max_pher > 0.01f && world_in_bounds(w, best_phero_pos)) {
      Cell *nc = &w->grid[best_phero_pos.y * w->width + best_phero_pos.x];
      // Even following pheromones, we might hit dirt if someone deposited scent
      // on it
      if (nc->type == CELL_RESOURCE && nc->resource.type == RESOURCE_DIRT) {
        ant->plan[0] = (Action){ACTION_PICKUP, best_phero_pos, {0}};
        ant->plan_length = 1;
        ant->state = STATE_CLEARING;
      } else if (nc->type == CELL_EMPTY) {
        ant->plan[0] = (Action){ACTION_MOVE, best_phero_pos, {0}};
        ant->plan_length = 1;
      }
    } else {
      // Lost food trail? Wander or revert to SCOUTING
      ant->state = STATE_SCOUTING;
    }

    break;
  }

  case STATE_RETURNING: {
    // 1. Check distance to nest
    float dist = sqrtf(dist_sq(ant->position, w->nest.position));
    if (dist <= w->nest.radius) {
      // Try to drop
      bool dropped = false;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          Position p = {ant->position.x + dx, ant->position.y + dy};
          if (world_in_bounds(w, p)) {
            Cell *c = &w->grid[p.y * w->width + p.x];
            if (c->type == CELL_EMPTY) {
              ant->plan[0] = (Action){ACTION_DROP, p, {0}};
              ant->plan_length = 1;
              ant->state = STATE_SCOUTING;
              dropped = true;
              goto done_returning;
            }
          }
        }
      }
    done_returning:
      if (dropped)
        return;

      // If no space, wander around nest (random move)
      int rx = (rand() % 3) - 1;
      int ry = (rand() % 3) - 1;
      Position rp = {ant->position.x + rx, ant->position.y + ry};
      if (world_in_bounds(w, rp)) {
        Cell *rc = &w->grid[rp.y * w->width + rp.x];
        if (rc->type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, rp, {0}};
          ant->plan_length = 1;
        }
      }
      return;
    }

    // 2. Follow Home Pheromone or Vision
    // If nest visible (radius), go direct
    if (dist <= obs_radius) {
      AntVector v = AntVector_from_positions(ant->position, w->nest.position);
      v = AntVector_normalize(v);
      int tx = ant->position.x + (int)(v.x + (v.x > 0 ? 0.5f : -0.5f));
      int ty = ant->position.y + (int)(v.y + (v.y > 0 ? 0.5f : -0.5f));
      Position next_step = {tx, ty};
      if (world_in_bounds(w, next_step)) {
        Cell *nc = &w->grid[next_step.y * w->width + next_step.x];
        if (nc->type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, next_step, {0}};
          ant->plan_length = 1;
          return;
        }
      }
    }

    // Follow Trail
    Position best_home = {-1, -1};
    float max_home = -1.0f;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->pheromone_to_home > max_home) {
            max_home = c->pheromone_to_home;
            best_home = p;
          }
        }
      }
    }

    if (max_home > 0.01f && world_in_bounds(w, best_home)) {
      Cell *nc = &w->grid[best_home.y * w->width + best_home.x];
      if (nc->type == CELL_EMPTY) {
        ant->plan[0] = (Action){ACTION_MOVE, best_home, {0}};
        ant->plan_length = 1;
      }
      ant->frustration = 0;
    } else {
      // Lost -> Increase frustration
      ant->frustration++;
      if (ant->frustration >
          100) { // e.g., 2-3 seconds at 60fps? Or just 10 steps?
        ant->state = STATE_SEARCHING_HOME;
        ant->frustration = 0;
      } else {
        // Wander slightly
        int rx = (rand() % 3) - 1;
        int ry = (rand() % 3) - 1;
        Position rp = {ant->position.x + rx, ant->position.y + ry};
        if (world_in_bounds(w, rp) &&
            w->grid[rp.y * w->width + rp.x].type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, rp, {0}};
          ant->plan_length = 1;
        }
      }
    }

    break;
  }

  case STATE_CLEARING: {
    if (!ant->is_carring) {
      ant->state = STATE_SCOUTING;
      break;
    }

    // Goal: Find Dead Zone (least pheromones)
    float min_phero = 100000.0f;
    Position best_spot = ant->position;
    bool found = false;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          float val = c->pheromone_to_food + c->pheromone_to_home +
                      c->pheromone_visited;
          if (val < min_phero) {
            min_phero = val;
            best_spot = p;
            found = true;
          }
        }
      }
    }

    if (found) {
      Cell *tc = &w->grid[best_spot.y * w->width + best_spot.x];
      if (tc->type == CELL_EMPTY) {
        // If very low activity, drop.
        if (min_phero < 0.5f) {
          ant->plan[0] = (Action){ACTION_DROP, best_spot, {0}};
          ant->plan_length = 1;
          ant->state = STATE_SCOUTING;
        } else {
          // Walk towards the darkness
          ant->plan[0] = (Action){ACTION_MOVE, best_spot, {0}};
          ant->plan_length = 1;
        }
      }
    }

    // Fallback if no plan
    if (ant->plan_length == 0) {
      int rx = (rand() % 3) - 1;
      int ry = (rand() % 3) - 1;
      Position rp = {ant->position.x + rx, ant->position.y + ry};
      if (world_in_bounds(w, rp)) {
        Cell *rc = &w->grid[rp.y * w->width + rp.x];
        if (rc->type == CELL_EMPTY) {
          ant->plan[0] = (Action){ACTION_MOVE, rp, {0}};
          ant->plan_length = 1;
        }
      }
    }
    break;
  }

  case STATE_SEARCHING_HOME: {
    // Timeout check
    ant->frustration++;
    if (ant->frustration > 1000) {
      // Drop food to survive (become scout)
      if (ant->is_carring) {
        ant->plan[0] = (Action){ACTION_DROP, ant->position, {0}};
        // Can only drop on empty neighbor, but drop action usually takes target
        // Let's find empty neighbor
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0)
              continue;
            Position p = {ant->position.x + dx, ant->position.y + dy};
            if (world_in_bounds(w, p) &&
                w->grid[p.y * w->width + p.x].type == CELL_EMPTY) {
              ant->plan[0] = (Action){ACTION_DROP, p, {0}};
              ant->plan_length = 1;
              ant->state = STATE_SCOUTING;
              ant->frustration = 0;
              return;
            }
          }
        }
      } else {
        ant->state = STATE_SCOUTING;
        ant->frustration = 0;
      }
    }

    // Random Walk + Spiral attempt (Spiral is hard, just expansive random)
    // Also check if we smell home
    float max_home = -1.0f;
    for (int dy = -2; dy <= 2; dy++) {
      for (int dx = -2; dx <= 2; dx++) {
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (world_in_bounds(w, p)) {
          if (w->grid[p.y * w->width + p.x].pheromone_to_home > max_home) {
            max_home = w->grid[p.y * w->width + p.x].pheromone_to_home;
          }
        }
      }
    }

    if (max_home > 0.1f) {
      ant->state = STATE_RETURNING;
      ant->frustration = 0;
      return;
    }

    // Move randomly
    int rx = (rand() % 3) - 1;
    int ry = (rand() % 3) - 1;
    Position rp = {ant->position.x + rx, ant->position.y + ry};
    if (world_in_bounds(w, rp) &&
        w->grid[rp.y * w->width + rp.x].type == CELL_EMPTY) {
      ant->plan[0] = (Action){ACTION_MOVE, rp, {0}};
      ant->plan_length = 1;
    }
    break;
  }
  }
}

// Simulation
void system_update_ant(int ant_id, World *world) {
  if (!world || ant_id < 0 || ant_id >= world->nest.num_ants) {
    return;
  }

  Ant *ant = &(world->nest.ants[ant_id]);
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
    if (!world_in_bounds(world, target)) {
      ant_clear_plan(ant);
      return;
    }
    Cell *tc = &world->grid[target.y * world->width + target.x];
    if (tc->type == CELL_RESOURCE || tc->type == CELL_NEST) {
      ant_clear_plan(ant);
      return;
    }

    Cell new_cell;
    new_cell.type = CELL_ANT;
    new_cell.ant_id = (ant_id >= 0) ? ant_id : 0;

    // Preserve existing pheromones from target cell
    new_cell.pheromone_to_food = tc->pheromone_to_food;
    new_cell.pheromone_to_home = tc->pheromone_to_home;
    new_cell.pheromone_visited = tc->pheromone_visited;

    // Deposit trails
    if (ant->is_carring && ant->carried_resource.type == RESOURCE_FOOD) {
      if (new_cell.pheromone_to_food < 1.0f)
        new_cell.pheromone_to_food = 1.0f;
    }

    // Deposit exploration trail
    if (new_cell.pheromone_visited < 1.0f)
      new_cell.pheromone_visited = 1.0f;

    new_cell.resource = (Resource){0};
    world_occupy_cell(world, target, new_cell);
    world_vacate_cell(world, ant->position);
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

    /* remove resource from world, preserving pheromones */
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

    // Drop logic
    Cell *c = &world->grid[target.y * world->width + target.x];
    if (c->type != CELL_EMPTY && c->type != CELL_NEST) {
      ant_clear_plan(ant);
      return;
    }

    bool is_nest = Position_equals(target, world->nest.position);
    if (c->type == CELL_NEST || is_nest) {
      // Consume
      ant->is_carring = false;
      ant->carried_resource = (Resource){0};
      return;
    }

    if (c->type != CELL_EMPTY) {
      ant_clear_plan(ant);
      return;
    }

    Cell drop;
    drop.type = CELL_RESOURCE;
    drop.resource = ant->carried_resource;
    if (drop.resource.type == RESOURCE_FOOD) {
      float dist_to_nest_sq = dist_sq(ant->position, world->nest.position);
      if (dist_to_nest_sq <= (world->nest.radius * world->nest.radius)) {
        world->nest.stored_food++;
        drop.resource.type = RESOURCE_RESERVOIR;
      }
    }

    // Preserve pheromones
    drop.pheromone_to_food = c->pheromone_to_food;
    drop.pheromone_to_home = c->pheromone_to_home;
    drop.pheromone_visited = c->pheromone_visited;
    drop.ant_id = -1;
    world_occupy_cell(world, target, drop);

    ant->is_carring = false;
    ant->carried_resource = (Resource){0};
    break;
  }
  }
}

void system_update_world(World *w) {
  if (!w) {
    return;
  }

  for (int i = 0; i < w->nest.num_ants; i++) {
    system_update_ant(i, w);
  }

  /* Pheromone diffusion + evaporation + source injection */
  int width = w->width;
  int height = w->height;
  int n = width * height;

  float *new_food = calloc(n, sizeof(float));
  float *new_home = calloc(n, sizeof(float));
  float *new_visited = calloc(n, sizeof(float));
  if (!new_food || !new_home || !new_visited) {
    free(new_food);
    free(new_home);
    free(new_visited);
    return;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;
      Cell *c = &w->grid[idx];

      int neigh[8];
      int nc = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          int nx = x + dx;
          int ny = y + dy;
          if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            continue;
          neigh[nc++] = ny * width + nx;
        }
      }

      float v_food = c->pheromone_to_food;
      float v_home = c->pheromone_to_home;
      float v_visited = c->pheromone_visited;

      float d_food = v_food * DIFFUSION_RATE;
      float d_home = v_home * DIFFUSION_RATE;
      float d_visited = v_visited * DIFFUSION_RATE;

      float keep_food = v_food - d_food;
      float keep_home = v_home - d_home;
      float keep_visited = v_visited - d_visited;

      new_food[idx] += keep_food;
      new_home[idx] += keep_home;
      new_visited[idx] += keep_visited;

      if (nc > 0) {
        float share_food = d_food / nc;
        float share_home = d_home / nc;
        float share_visited = d_visited / nc;
        for (int k = 0; k < nc; k++) {
          int ni = neigh[k];
          new_food[ni] += share_food;
          new_home[ni] += share_home;
          new_visited[ni] += share_visited;
        }
      } else {
        new_food[idx] += d_food;
        new_home[idx] += d_home;
        new_visited[idx] += d_visited;
      }
    }
  }

  /* evaporation */
  for (int i = 0; i < n; i++) {
    new_food[i] *= EVAPORATION_RATE;
    new_home[i] *= EVAPORATION_RATE;
    new_visited[i] *= EVAPORATION_RATE;
  }

  for (int i = 0; i < n; i++) {
    w->grid[i].pheromone_to_food = new_food[i];
    w->grid[i].pheromone_to_home = new_home[i];
    w->grid[i].pheromone_visited = new_visited[i];
  }

  if (world_in_bounds(w, w->nest.position)) {
    int ni = w->nest.position.y * width + w->nest.position.x;
    w->grid[ni].pheromone_to_home = 1.0f;
  }
  for (int i = 0; i < n; i++) {
    if (w->grid[i].type == CELL_RESOURCE &&
        w->grid[i].resource.type == RESOURCE_FOOD) {
      w->grid[i].pheromone_to_food = 1.0f;
    }
  }

  free(new_food);
  free(new_home);
  free(new_visited);

  // check nest radius update
  int nest_radius_sq = w->nest.radius * w->nest.radius;
  int food_within_radius = 0;
  for (int y = -w->nest.radius; y <= w->nest.radius; y++) {
    for (int x = -w->nest.radius; x <= w->nest.radius; x++) {
      if (x * x + y * y <= nest_radius_sq) {
        Position p = {w->nest.position.x + x, w->nest.position.y + y};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[p.y * w->width + p.x];
          if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
            food_within_radius++;
          }
        }
      }
    }
  }

  nest_set_food(&w->nest, food_within_radius);
  nest_update_radius(&w->nest);
}