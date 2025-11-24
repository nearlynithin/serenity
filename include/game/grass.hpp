#ifndef GRASS_H
#define GRASS_H

#define MAX_INSTANCES 10000

#include "raylib.h"
#include "raymath.h"
#include "renderContext.hpp"
#include "shader.hpp"
//#include "utils.h"
#include <set>

class Grass
{
  private:
    std::set<Vector2> grassPos;
    Model grass;
    Matrix *transforms;
    Material matInstances;
    int totalInstances;

  public:
    void InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height);
    void DrawGrass(Camera3D &camera, SceneContext *sceneCtx);
    void Animate(gfx::Shader *grassShader);
    void UnloadGrass();
};

#endif
