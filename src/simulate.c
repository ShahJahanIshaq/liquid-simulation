#include "simulate.h"
#include "config.h"
#include <math.h>
#include <string.h>
#include <time.h>


static inline float clampf(float v,float a,float b)
{ return (v < a) ? a : (v > b) ? b : v; }

static inline bool supported(const Cell *g,int x,int y)
{
    if (y == ROWS - 1) return true;
    const Cell *below = &g[idx(x,y+1)];
    return below->type == CELL_SOLID ||
           (below->type == CELL_WATER && below->fluid > 0.f);
}


static void settle(Cell *g)
{
    for (int y = ROWS - 2; y >= 0; --y)
        for (int x = 0; x < COLUMNS; ++x) {
            Cell *src = &g[idx(x,y)];
            if (src->type != CELL_WATER || src->fluid <= 0.f) continue;

            Cell *dst = &g[idx(x,y+1)];
            if (dst->type == CELL_SOLID) continue;

            float dstFluid = (dst->type == CELL_WATER) ? dst->fluid : 0.f;
            float room     = 1.f - dstFluid;
            if (room <= 0.f) continue;

            float moved = fminf(src->fluid, room);

            dst->type  = CELL_WATER;
            dst->fluid = dstFluid + moved;

            src->fluid -= moved;
            if (src->fluid <= 1e-4f) *src = (Cell){ CELL_EMPTY, 0.f };
        }
}


static void taps_emit(Cell *g,float amount)
{
    if (amount <= 0.f) return;
    for (int y=0; y<ROWS; ++y)
        for (int x=0; x<COLUMNS; ++x) {
            if (g[idx(x,y)].type != CELL_TAP) continue;
            if (y+1 >= ROWS) continue;

            Cell *dst = &g[idx(x,y+1)];
            if (dst->type == CELL_SOLID || dst->type == CELL_TAP) continue;

            float dstFluid = (dst->type == CELL_WATER) ? dst->fluid : 0.f;
            float moved    = fminf(amount, 1.f - dstFluid);
            if (moved <= 0.f) continue;

            dst->type  = CELL_WATER;
            dst->fluid = dstFluid + moved;
        }
}


static void shuffle_indices(int *a,int n)
{
    for (int i=n-1;i>0;--i){
        int j = rand() % (i+1);
        int t = a[i]; a[i]=a[j]; a[j]=t;
    }
}


static void simulate(Cell *grid,Cell *buf,int *order,int count)
{
    memcpy(buf, grid, sizeof(Cell)*count);
    shuffle_indices(order, count);

    for (int n=0;n<count;++n){
        int i  = order[n];
        int x  = i % COLUMNS;
        int y  = i / COLUMNS;

        const Cell *srcRO = &grid[i];
        if (srcRO->type != CELL_WATER || srcRO->fluid <= 0.f) continue;

        Cell *src = &buf[i];
        float fluidLeft = src->fluid;

        // gravity
        if (y+1 < ROWS && fluidLeft > 0.f){
            Cell *d = &buf[idx(x,y+1)];
            if (d->type != CELL_SOLID){
                float dstFluid = (d->type==CELL_WATER)?d->fluid:0.f;
                float moved    = fminf(fluidLeft, 1.f-dstFluid);
                if (moved>0.f){
                    d->type  = CELL_WATER;
                    d->fluid = dstFluid + moved;
                    fluidLeft -= moved;
                }
            }
        }

        // left and right
        if (fluidLeft > 0.f && supported(buf,x,y)){
            for (int dir=-1; dir<=1 && fluidLeft>0.f; dir+=2){
                int nx = x+dir;
                if (nx<0 || nx>=COLUMNS) continue;

                Cell *neigh = &buf[idx(nx,y)];
                if (neigh->type == CELL_SOLID) continue;

                float nFluid = (neigh->type==CELL_WATER)?neigh->fluid:0.f;
                float diff   = fluidLeft - nFluid;
                if (diff <= 0.01f) continue;

                float moved = fminf(diff*0.5f, fminf(fluidLeft,1.f-nFluid));
                if (moved>0.f){
                    neigh->type  = CELL_WATER;
                    neigh->fluid = nFluid + moved;
                    fluidLeft   -= moved;
                }
            }
        }


        if (fluidLeft <= 1e-4f)
            *src = (Cell){ CELL_EMPTY, 0.f };
        else
            src->fluid = fluidLeft;
    }


    memcpy(grid, buf, sizeof(Cell)*count);
    settle(grid);
}


void sim_step(World *w,float tapRate)
{
    static Cell *buf   = NULL;
    static int  *order = NULL;
    static bool  ready = false;

    if (!ready){
        buf   = (Cell*)malloc(sizeof(Cell)*ROWS*COLUMNS);
        order = (int*) malloc(sizeof(int)*ROWS*COLUMNS);
        for (int i=0;i<ROWS*COLUMNS;++i) order[i]=i;
        srand((unsigned)time(NULL));
        ready = true;
    }

    taps_emit(w->data, tapRate);
    simulate(w->data, buf, order, ROWS*COLUMNS);
}