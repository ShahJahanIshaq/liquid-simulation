#include "../include/SDL2/SDL.h"
#include "input.h"
#include "config.h"
#include <math.h>
#include <stdio.h>

void input_handle_events(InputState *st,World *w,bool *running)
{
    SDL_Event ev;
    if(SDL_PollEvent(&ev)){
        switch(ev.type){
        case SDL_QUIT: *running=false; break;

        case SDL_KEYDOWN:{
            SDL_Keycode k = ev.key.keysym.sym;
            switch(k){
            case SDLK_w: st->brush = CELL_WATER; break;
            case SDLK_s: st->brush = CELL_SOLID; break;
            case SDLK_e: st->brush = CELL_EMPTY; break;
            case SDLK_t: st->brush = CELL_TAP;   break;


            case SDLK_MINUS: case SDLK_KP_MINUS:
                st->tapRate = fmaxf(0.f, st->tapRate - 0.05f);
                printf("tap rate: %.2f\n",st->tapRate); break;
            case SDLK_EQUALS: case SDLK_PLUS: case SDLK_KP_PLUS:
                st->tapRate = fminf(1.f, st->tapRate + 0.05f);
                printf("tap rate: %.2f\n",st->tapRate); break;

            case SDLK_ESCAPE: *running=false; break;
            default: break;
            }
        } break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:{
            bool lmb = (ev.type==SDL_MOUSEBUTTONDOWN) ||
                       (ev.type==SDL_MOUSEMOTION &&
                        (ev.motion.state & SDL_BUTTON_LMASK));
            if (!lmb) break;

            int cx = ev.motion.x / CELL_SIZE;
            int cy = ev.motion.y / CELL_SIZE;
            if (cx<0||cx>=COLUMNS||cy<0||cy>=ROWS) break;

            Cell *dst = cell(w,cx,cy);


            if (st->brush == CELL_EMPTY){
                *dst = (Cell){ CELL_EMPTY, 0.f };
                break;
            }


            if (dst->type == CELL_SOLID && st->brush != CELL_SOLID) break;

            *dst = (Cell){ st->brush, (st->brush==CELL_WATER)?1.f:0.f };
        } break;
        }
    }
}