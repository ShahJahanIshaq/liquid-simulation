#pragma once
#include "world.h"


   // 1. emit water from all taps
   // 2. run the fluid solver
void sim_step(World *w, float tapRate);