#ifndef GRASS_H
#define GRASS_H

#define MAX_INSTANCES 50000

#include "raylib.h"
#include "raymath.h"
#include "terrain.hpp"
#include <vector>

class Grass
{
  private:
    static std::vector<position> grassPos;
    static Model grass;
    static Matrix *transforms;
    static Material matInstances;
    static long count;

  public:
    static void InitGrass();
    static void DrawGrass();
    static void UnloadGrass();
    static position getGrassPosition();
};

#endif