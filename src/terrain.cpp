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
#define MAX_TERRAIN 20

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
    UnloadModel(terrain);
    // Terrain objects are automatically destroyed because of unique_ptr
}

// Terrain manager
void TerrainManager::LoadTerrains()
{
    for (int i = -MAX_TERRAIN / 2; i < MAX_TERRAIN / 2; i++)
    {
        for (int j = -MAX_TERRAIN; j < MAX_TERRAIN / 2; j++)
        {
            Vector2 pos = Vector2{static_cast<float>(i), static_cast<float>(j)};
            if (terrains.count(pos) == 0)
            {
                terrains[pos] = std::make_unique<Terrain>(i * WIDTH, j * HEIGHT);
            }
        }
    }
    currentTerrain = Vector2{0, 0};
    updateCords(currentTerrain);
}

void TerrainManager::DrawTerrains()
{
    for (auto &cord : cords)
    {
        Vector2 pos = Vector2{cord.x, cord.y};
        DrawModel(terrains[pos]->getTerrain(), terrains[pos]->getPosition(), 1, WHITE);
    }
}

void TerrainManager::UpdateTerrains()
{
    Player &player = Player::getInstance();
    Vector2 pos = player.getPlayerCords();
    if (pos != currentTerrain)
    {
        updateCords(pos);
    }
    currentTerrain = pos;
}
void TerrainManager::DrawTerrainGrid()
{
    DrawGrid(10, 50);
}

void TerrainManager::updateCords(Vector2 &pos)
{
    cords.clear();
    cords.insert(pos);
    cords.insert(Vector2{pos.x, pos.y + 1});
    cords.insert(Vector2{pos.x, pos.y - 1});
    cords.insert(Vector2{pos.x - 1, pos.y});
    cords.insert(Vector2{pos.x + 1, pos.y});
    cords.insert(Vector2{pos.x - 1, pos.y + 1});
    cords.insert(Vector2{pos.x + 1, pos.y - 1});
    cords.insert(Vector2{pos.x + 1, pos.y + 1});
    cords.insert(Vector2{pos.x - 1, pos.y - 1});
}