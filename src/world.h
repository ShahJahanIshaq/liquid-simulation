#pragma once
#include "config.h"
#include <stdbool.h>
#include <stdlib.h>


typedef enum { CELL_EMPTY, CELL_WATER, CELL_SOLID, CELL_TAP } CellType;

typedef struct {
    CellType type;
    float    fluid;  // 0 to 1
} Cell;


typedef struct {
    Cell *data;
} World;


static inline bool world_create(World *w)
{
    w->data = (Cell*)calloc(ROWS * COLUMNS, sizeof(Cell));
    return w->data != NULL;
}

static inline void world_destroy(World *w)
{
    free(w->data);
    w->data = NULL;
}

static inline void world_clear(World *w)
{
    for (int i = 0; i < ROWS * COLUMNS; ++i)
        w->data[i] = (Cell){ CELL_EMPTY, 0.f };
}


static inline int  idx(int x,int y) { return y * COLUMNS + x; }

static inline Cell* cell(World *w,int x,int y)
{ return &w->data[idx(x,y)]; }