#ifndef TERRAIN_H
#define TERRAIN_H

#include "game/grass.hpp"
#include "raylib.h"
#include "rlFrustum.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define WIDTH 50.0f
#define HEIGHT 50.0f
#define RES_X 80
#define RES_Y 80
#define MAX_TERRAIN 10
#define HEIGHT_FACTOR 100.0f
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
    Vector3 getPosition();
    void setTexture();
    void setShader();
    BoundingBox &getBBox();
    void DrawGrass(Camera3D &camera);
};

class TerrainManager
{
  private:
    static std::unordered_map<Vector2, std::unique_ptr<Terrain>> terrains;
    static std::unordered_set<Vector2> cords;
    static Vector2 currentTerrain;
    static RayCollision collision;
    static Ray playerMarker;
    static Grass grass;
    static RLFrustum frustum;

  public:
    static void LoadTerrains();
    static void DrawTerrains();
    static void DrawTerrainGrid();
    // static void updateCords(Vector2 &pos);
    // static void UpdateTerrains();
    static void UpdateCollision();
};

#endif