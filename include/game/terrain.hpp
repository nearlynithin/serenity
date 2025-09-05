#ifndef TERRAIN_H
#define TERRAIN_H

#include "game/grass.hpp"
#include "raylib.h"
#include "renderContext.hpp"
#include "rlFrustum.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define WIDTH 20.0f
#define HEIGHT 20.0f
#define RES_X 20
#define RES_Y 20
#define MAX_TERRAIN 20
#define HEIGHT_FACTOR 85.0f
#define NOISE_SCALE 10.0f

class Terrain
{
  private:
    float *vertexData;
    int resX;
    int resY;
    Mesh mesh;
    Model terrain;
    Vector3 position;
    BoundingBox bbox;
    Grass grass;

  public:
    Terrain(float offestx, float offesty);
    virtual ~Terrain();
    Model &getTerrain();
    Grass &getGrass();
    Vector3 getPosition();
    void setTexture();
    BoundingBox &getBBox();
    void DrawGrass(Camera3D &camera, SceneContext *sceneCtx);
};

class TerrainManager
{
  private:
    static std::unordered_map<Vector2, std::unique_ptr<Terrain>> terrains;
    static std::unordered_set<Vector2> cords;
    static Vector2 currentTerrain;
    static RayCollision collision;
    static Ray playerMarker;
    static RLFrustum frustum;
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};

  public:
    static void LoadTerrains();
    static void setShaders(SceneContext *sceneCtx);
    static void DrawTerrains(SceneContext *sceneCtx, bool shadowPass);
    static void DrawTerrainGrid();
    static void UpdateCollision();
};

#endif
