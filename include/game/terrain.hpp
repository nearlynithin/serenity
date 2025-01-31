#ifndef TERRAIN_H
#define TERRAIN_H

#include "raylib.h"

class Terrain
{
  private:
    float heightMultiplier;
    float noiseScale;
    float *vertexData;
    int resX;
    int resY;
    Mesh mesh;
    Model terrain;

  public:
    Terrain(float width = 1000, float length = 1000, float offestx = 0.0f, float offesty = 0.0f);
    virtual ~Terrain();
    void updateTerrain(float time, float offestx, float offesty);
    Model &getTerrain();
    void DrawTerrain();
};

#endif