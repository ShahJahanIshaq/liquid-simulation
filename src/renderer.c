#include "../include/SDL2/SDL.h"
#include "render.h"
#include "config.h"
#include <math.h>

typedef struct {
    Uint32 empty, solid, grid, water, tap;
} Palette;


struct Renderer {
    SDL_Window  *win;
    SDL_Surface *surf;
    Palette      pal;
};


static Palette make_palette(SDL_Surface *s)
{
    Palette p;
    p.empty = SDL_MapRGB(s->format, 0x00, 0x00, 0x00);
    p.solid = SDL_MapRGB(s->format, 0xFF, 0xFF, 0xFF);
    p.grid  = SDL_MapRGB(s->format, 0x1F, 0x1F, 0x1F);
    p.water = SDL_MapRGB(s->format, 0x34, 0xC3, 0xEB);
    p.tap   = SDL_MapRGB(s->format, 0x80, 0xC3, 0xFF);
    return p;
}


bool renderer_create(Renderer **out)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return false;
    }

    Renderer *r = (Renderer *)calloc(1, sizeof(Renderer));

    r->win = SDL_CreateWindow("Water Simulation",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!r->win) {
        SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        free(r);
        return false;
    }

    r->surf = SDL_GetWindowSurface(r->win);
    if (!r->surf) {
        SDL_Log("SDL_GetWindowSurface: %s", SDL_GetError());
        SDL_DestroyWindow(r->win);
        SDL_Quit();
        free(r);
        return false;
    }

    r->pal = make_palette(r->surf);
    *out   = r;
    return true;
}


void renderer_destroy(Renderer *r)
{
    if (!r) return;
    SDL_DestroyWindow(r->win);
    SDL_Quit();
    free(r);
}


static void draw_cell(SDL_Surface *s, const Renderer *r,
                      int x, int y, const Cell *c)
{
    SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };

    SDL_FillRect(s, &rect, r->pal.empty);

    switch (c->type) {
    case CELL_SOLID:
        SDL_FillRect(s, &rect, r->pal.solid);
        break;

    case CELL_TAP:
        SDL_FillRect(s, &rect, r->pal.tap);
        break;

    case CELL_WATER:
        if (c->fluid > 0.f) {
            int h = (int)lroundf(fminf(c->fluid, 1.f) * CELL_SIZE);
            SDL_Rect wrect = rect;
            wrect.y += CELL_SIZE - h;
            wrect.h  = h;
            SDL_FillRect(s, &wrect, r->pal.water);
        }
        break;

    default:
        break;
    }
}

static void draw_grid(SDL_Surface *s, const Renderer *r)
{
    for (int x = 0; x < COLUMNS; ++x) {
        SDL_Rect col = { x * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT };
        SDL_FillRect(s, &col, r->pal.grid);
    }
    for (int y = 0; y < ROWS; ++y) {
        SDL_Rect row = { 0, y * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH };
        SDL_FillRect(s, &row, r->pal.grid);
    }
}


void renderer_draw(Renderer *r, const World *w)
{
    SDL_Surface *s = r->surf;

    for (int y = 0; y < ROWS; ++y)
        for (int x = 0; x < COLUMNS; ++x)
            draw_cell(s, r, x, y, &w->data[idx(x, y)]);

    draw_grid(s, r);
    SDL_UpdateWindowSurface(r->win);
}