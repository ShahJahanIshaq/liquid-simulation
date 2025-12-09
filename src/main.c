#define SDL_MAIN_HANDLED
#include "../include/SDL2/SDL.h"
#include "config.h"
#include "world.h"
#include "simulate.h"
#include "render.h"
#include "input.h"
#include <stdbool.h>

int main(void)
{
    World    world;
    Renderer *ren  = NULL;
    InputState in  = { .brush = CELL_SOLID, .tapRate = TAP_RATE_INITIAL };
    bool running   = world_create(&world) && renderer_create(&ren);

    while(running){
        input_handle_events(&in,&world,&running);
        sim_step(&world,in.tapRate);
        renderer_draw(ren,&world);
        SDL_Delay(10);
    }

    renderer_destroy(ren);
    world_destroy(&world);
    return 0;
}