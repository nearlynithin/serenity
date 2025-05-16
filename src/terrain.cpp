#include "game/terrain.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raymath.h"
#include "utils.hpp"
#include <stdlib.h>
#include <string.h>
#include <unordered_set>
#define WIDTH 50.0f
#define HEIGHT 50.0f
#define RES_X 100
#define RES_Y 100
#define SCALE 100.0f

// std::vector<std::unique_ptr<Terrain>> TerrainManager::terrains;
std::unordered_map<Vector2, std::unique_ptr<Terrain>> TerrainManager::terrains;
std::vector<position> TerrainManager::grassPositions;
Vector2 TerrainManager::currentTerrain;
std::unordered_set<Vector2> TerrainManager::cords;

Terrain::Terrain(float offsetx, float offsety)
  : heightMultiplier(25.0f),
    noiseScale(10.0f),
    resX(RES_X),
    resY(RES_Y),
    position(Vector3{WIDTH / 2 + offsetx, 0, HEIGHT / 2 + offsety})
{
    mesh = GenMeshPlane(WIDTH, HEIGHT, resX, resY);
    vertexData = (float *)malloc(mesh.vertexCount * 3 * sizeof(float));
    memcpy(vertexData, mesh.vertices, mesh.vertexCount * 3 * sizeof(float));
    terrain = LoadModelFromMesh(mesh);

    for (int i = 0; i < mesh.vertexCount * 3; i += 3)
    {
        float scaledX = vertexData[i] * noiseScale;
        float scaledZ = vertexData[i + 2] * noiseScale;

        float height = terrainNoise(scaledX + offsetx * 10, scaledZ + offsety * 10, 100.0f);
        vertexData[i + 1] = height * heightMultiplier;
    }
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);
    setTexture();
    setShader();
}
Model &Terrain::getTerrain()
{
    return terrain;
}
Vector3 Terrain::getPosition()
{
    return position;
}

void Terrain::setTexture()
{
    Texture2D tex = ResourceManager::getInstance().GetTexture("terrainTexture");
    terrain.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
    GenTextureMipmaps(&tex);
    SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
}

void Terrain::setShader()
{
    // Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    // terrain.materials[0].shader = shadow_shader;
}

Terrain::~Terrain()
{
    free(vertexData);
    // Terrain objects are automatically destroyed because of unique_ptr
}

void Terrain::updateTerrain(float time, float offsetx, float offsety)
{
    for (int i = 0; i < mesh.vertexCount * 3; i += 3)
    {
        float scaledX = vertexData[i] * noiseScale;
        float scaledZ = vertexData[i + 2] * noiseScale;

        float height = terrainNoise(scaledX + offsetx, scaledZ + offsety + time, SCALE);
        vertexData[i + 1] = height * heightMultiplier;
    }
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);
}

// Terrain manager
void TerrainManager::LoadTerrains()
{
    terrains[Vector2{0, 0}] = std::make_unique<Terrain>(0.0f, 0.0f);
    terrains[Vector2{0, 1}] = std::make_unique<Terrain>(0.0f, 50.0f);
    terrains[Vector2{0, -1}] = std::make_unique<Terrain>(0.0f, -50.0f);
    terrains[Vector2{-1, 0}] = std::make_unique<Terrain>(-50.0f, 0.0f);
    terrains[Vector2{1, 0}] = std::make_unique<Terrain>(50.0f, 0.0f);
    terrains[Vector2{-1, 1}] = std::make_unique<Terrain>(-50.0f, 50.0f);
    terrains[Vector2{1, -1}] = std::make_unique<Terrain>(50.0f, -50.0f);
    terrains[Vector2{1, 1}] = std::make_unique<Terrain>(50.0f, 50.0f);
    terrains[Vector2{-1, -1}] = std::make_unique<Terrain>(-50.0f, -50.0f);
    currentTerrain = Vector2{0, 0};
}

void TerrainManager::DrawTerrains()
{
    for (auto &terrain : terrains)
    {
        DrawModel(terrain.second->getTerrain(), terrain.second->getPosition(), 1, WHITE);
    }

    rlDisableWireMode();
}

void TerrainManager::UpdateTerrains()
{
    getCords();
    if (!cords.empty())
    {
        for (const auto &cord : cords)
        {
            if (terrains.count(cord) == 0)
            {
                terrains[cord] = std::make_unique<Terrain>(cord.x * WIDTH, cord.y * HEIGHT);
            }
        }

        for (auto it = terrains.begin(); it != terrains.end();)
        {
            if (cords.count(it->first) == 0)
            {
                it = terrains.erase(it);
            }
            else
            {
                it++;
            }
        }

        cords.clear();
    }
}
void TerrainManager::DrawTerrainGrid()
{
    DrawGrid(10, 50);
}

void TerrainManager::getCords()
{
    Player &player = Player::getInstance();
    Vector2 pos = player.getPlayerCords();
    if (pos != currentTerrain)
    {
        cords.insert(pos);
        cords.insert(Vector2{pos.x, pos.y + 1});
        cords.insert(Vector2{pos.x, pos.y - 1});
        cords.insert(Vector2{pos.x - 1, pos.y});
        cords.insert(Vector2{pos.x + 1, pos.y});
        cords.insert(Vector2{pos.x - 1, pos.y + 1});
        cords.insert(Vector2{pos.x + 1, pos.y - 1});
        cords.insert(Vector2{pos.x + 1, pos.y + 1});
        cords.insert(Vector2{pos.x - 1, pos.y - 1});
        currentTerrain = pos;
    }
}