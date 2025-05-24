#ifndef TERRAIN_H
#define TERRAIN_H

#include "raylib.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define WIDTH 50.0f
#define HEIGHT 50.0f
#define RES_X 100
#define RES_Y 100
#define MAX_TERRAIN 20

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
    Vector3 position;
    BoundingBox bbox;

  public:
    Terrain(float offestx, float offesty);
    virtual ~Terrain();
    Model &getTerrain();
    Vector3 getPosition();
    void setTexture();
    void setShader();
    BoundingBox &getBBox();
};

class TerrainManager
{
  private:
    static std::unordered_map<Vector2, std::unique_ptr<Terrain>> terrains;
    static std::unordered_set<Vector2> cords;
    static Vector2 currentTerrain;
    static RayCollision collision;
    static Ray playerMarker;

  public:
    static std::vector<position> grassPositions;
    static void LoadTerrains();
    static void DrawTerrains();
    static Mesh *getTerrainVertices(int index);
    static void DrawTerrainGrid();
    static void updateCords(Vector2 &pos);
    static void UpdateTerrains();
    static void UpdateCollision();
};

#endif