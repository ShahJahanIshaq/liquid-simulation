#pragma once
#include "world.h"
#include <stdbool.h>

typedef struct Renderer Renderer;


bool renderer_create(Renderer **r);
void renderer_destroy(Renderer *r);


void renderer_draw(Renderer *r, const World *w);