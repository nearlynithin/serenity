#ifndef TERRAIN_H
#define TERRAIN_H

#include "raylib.h"
#include <memory>
#include <vector>

typedef struct
{
    float x;
    float y;
    float z;
} position;

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
    Terrain(float offestx, float offesty);
    virtual ~Terrain();
    void updateTerrain(float time, float offestx, float offesty);
    Model &getTerrain();
    void DrawTerrain();
    void setTexture();
    void setShader();g
};

class TerrainManager
{
  private:
    static std::vector<std::unique_ptr<Terrain>> terrains;

  public:
    static std::vector<position> grassPositions;
    static void LoadTerrains();
    static void DrawTerrains();g
    static Mesh *getTerrainVertices(int index);
};

#endif