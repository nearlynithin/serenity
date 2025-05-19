#ifndef TERRAIN_H
#define TERRAIN_H

#include "raylib.h"
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
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
    Vector3 position;

  public:
    Terrain(float offestx, float offesty);
    virtual ~Terrain();
    void updateTerrain(float time, float offestx, float offesty);
    Model &getTerrain();
    Vector3 getPosition();
    void setTexture();
    void setShader();
};

class TerrainManager
{
  private:
    // static std::vector<std::unique_ptr<Terrain>> terrains;
    static std::unordered_map<Vector2, std::unique_ptr<Terrain>> terrains;
    static std::unordered_set<Vector2> cords;
    static Vector2 currentTerrain;
    // static std::thread t_updater;
    static std::mutex t_mutex;
    

  public:
    static std::vector<position> grassPositions;
    static void LoadTerrains();
    static void DrawTerrains();
    static Mesh *getTerrainVertices(int index);
    static void DrawTerrainGrid();
    static bool getCords();
    static void UpdateTerrains();
    static void TerrainUpdater();
    static void setTerrainUpdater();
};

#endif