#ifndef GRASS_H
#define GRASS_H

#define MAX_INSTANCES 30000

#include "raylib.h"
#include "raymath.h"
#include "utils.h"
#include <set>

class Grass
{
  private:
    std::set<Vector2> grassPos;
    Model grass;
    Matrix *transforms;
    Material matInstances;

  public:
    void InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height,
                   Mesh &mesh);
    void DrawGrass();
    void UnloadGrass();
};

#endif