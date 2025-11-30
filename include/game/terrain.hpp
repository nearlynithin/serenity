#ifndef TERRAIN_H
#define TERRAIN_H

#include "game/grass.hpp"
#include "player.hpp"
#include "raylib.h"
#include "renderContext.hpp"
#include "rlFrustum.h"
#include "utils.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

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
    std::unordered_map<Vector2, std::unique_ptr<Terrain>> terrains;
    std::unordered_set<Vector2> cords;
    Vector2 currentTerrain;
    RayCollision collision;
    Ray playerMarker;
    RLFrustum frustum;
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};

  public:
    void LoadTerrains(Player &player);
    void setShaders(SceneContext *sceneCtx);
    void DrawTerrains(SceneContext *sceneCtx, bool shadowPass, Player &player);
    void DrawTerrainGrid();
    void UpdateCollision(Player &player);
};

#endif
