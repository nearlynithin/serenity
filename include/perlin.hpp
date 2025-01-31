#ifndef PERLIN_H
#define PERLIN_H

#include "raylib.h"
#include <math.h>


Vector2 randomGradient(int ix, int iy);
float dotGridGradient(int ix, int iy, float x, float y);
float interpolate(float a0, float a1, float w);
float smoothstep(float t);
float perlin(float worldX, float worldZ, float scale);
float smoothstep(float t);

#endif