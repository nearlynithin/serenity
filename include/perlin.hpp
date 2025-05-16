#ifndef PERLIN_H
#define PERLIN_H

#include "raylib.h"
#include <math.h>

Vector2 randomGradient(int ix, int iy);
float dotGridGradient(int ix, int iy, float x, float y);
float interpolate(float a0, float a1, float w);
float perlin(float x, float y, float scale, int octaves = 8, float persistence = 0.5f, float lacunarity = 2.0f);
float singlePerlin(float x, float y);
float smoothStep(float t);
float terrainNoise(float x, float y, float scale);

#endif