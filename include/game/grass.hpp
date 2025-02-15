#ifndef GRASS_H
#define GRASS_H

#define MAX_INSTANCES 12000

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "terrain.hpp"

class Grass
{
  private:
    static std::vector<position> grassPos;
    static Model grass;
    static Matrix *transforms;
    static Material matInstances;
    static int count;

  public:
    static void InitGrass();
    static void DrawGrass();
    static void UnloadGrass();
    static position getGrassPosition();
};

#endif