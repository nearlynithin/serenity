#ifndef GRASS_H
#define GRASS_H

#define MAX_INSTANCES 10000

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
    int timeLoc;
    int camPosLoc;
    int camTargetLoc;
    int ambientLoc;
    float fogDensityLoc;
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float fogDensity = 0.45f;

    void InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height);
    void DrawGrass(Camera3D &camera);
    void Animate();
    void UnloadGrass();
};

#endif
