#include "simulation.h"
#include "ant_math.h"
#include "ant_world.h"
#include <stdlib.h>
#include <string.h>

int view_init(LocalView *view, int radius) {
  if (!view || radius < 0) {
    return -1;
  }

  int diameter = 2 * radius + 1;
  view->radius = radius;
  view->diameter = diameter;

  view->obstacles = malloc(sizeof(bool *) * diameter);
  if (!view->obstacles) {
    return -1;
  }

  view->food_scent = malloc(sizeof(int *) * diameter);
  if (!view->food_scent) {
    free(view->obstacles);
    view->obstacles = NULL;
    return -1;
  }

  for (int i = 0; i < diameter; i++) {
    view->obstacles[i] = calloc(diameter, sizeof(bool));
    view->food_scent[i] = calloc(diameter, sizeof(int));
    if (!view->obstacles[i] || !view->food_scent[i]) {
      for (int j = 0; j <= i; j++) {
        if (view->obstacles[j])
          free(view->obstacles[j]);
        if (view->food_scent[j])
          free(view->food_scent[j]);
      }
      free(view->obstacles);
      free(view->food_scent);
      view->obstacles = NULL;
      view->food_scent = NULL;
      return -1;
    }
  }

  return 0;
}

void view_free(LocalView *view) {
  if (!view) {
    return;
  }

  if (view->obstacles) {
    for (int i = 0; i < view->diameter; i++) {
      free(view->obstacles[i]);
    }
    free(view->obstacles);
    view->obstacles = NULL;
  }

  if (view->food_scent) {
    for (int i = 0; i < view->diameter; i++) {
      free(view->food_scent[i]);
    }
    free(view->food_scent);
    view->food_scent = NULL;
  }

  view->radius = 0;
  view->diameter = 0;
}

void view_populate(LocalView *view, const World *world, Position center) {
  if (!view || !world || !view->obstacles) {
    return;
  }

  int r = view->radius;

  for (int dy = -r; dy <= r; dy++) {
    for (int dx = -r; dx <= r; dx++) {
      Position world_pos = {center.x + dx, center.y + dy};
      int view_x = dx + r;
      int view_y = dy + r;

      if (world_in_bounds(world, world_pos)) {
        view->obstacles[view_y][view_x] = world_is_occupied(world, world_pos);
      } else {
        view->obstacles[view_y][view_x] = true;
      }

      view->food_scent[view_y][view_x] = 0;
    }
  }
}

void system_update_ant(Ant *ant, World *world) {
  if (!ant || !world) {
    return;
  }

  int radius = ANT_DEFAULT_RADIUS;
  LocalView view;
  if (view_init(&view, radius) != 0) {
    return;
  }

  view_populate(&view, world, ant->position);
  ant_think(ant, &view);

  Action action;
  if (ant_get_current_action(ant, &action)) {
    switch (action.type) {
    case ACTION_IDLE:
      ant_advance_plan(ant);
      break;

    case ACTION_MOVE_TO: {
      int dx = action.target.x - ant->position.x;
      int dy = action.target.y - ant->position.y;

      if (dx > 1)
        dx = 1;
      if (dx < -1)
        dx = -1;
      if (dy > 1)
        dy = 1;
      if (dy < -1)
        dy = -1;

      if (dx == 0 && dy == 0) {
        ant_advance_plan(ant);
      } else {
        Position step = {dx, dy};
        Position next_pos = {ant->position.x + dx, ant->position.y + dy};

        ant->orientation = offset_to_orientation(step);
        if (world_in_bounds(world, next_pos) &&
            !world_is_occupied(world, next_pos)) {
          world_vacate_cell(world, ant->position);
          world_occupy_cell(world, next_pos);
          ant->position = next_pos;

          if (ant->position.x == action.target.x &&
              ant->position.y == action.target.y) {
            ant_advance_plan(ant);
          }
        } else {
          ant_clear_plan(ant);
        }
      }
      break;
    }

    case ACTION_PICKUP:
      if (action.resource) {
        ant->carried_resource = action.resource;
        world_vacate_cell(world, action.resource->position);
      }
      ant_advance_plan(ant);
      break;

    case ACTION_DROP:
      if (ant->carried_resource) {
        ant->carried_resource->position = action.target;
        world_occupy_cell(world, action.resource->position);
        ant->carried_resource = NULL;
      }
      ant_advance_plan(ant);
      break;

    default:
      ant_advance_plan(ant);
      break;
    }
  }

  view_free(&view);

  if (ant->life > 0) {
    ant->life--;
  }
}
