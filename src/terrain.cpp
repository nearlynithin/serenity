#include "game/terrain.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>
#define WIDTH 50
#define HEIGHT 50
#define RES_X 100
#define RES_Y 100
#define SCALE 100.0f

std::vector<std::unique_ptr<Terrain>> TerrainManager::terrains;
std::vector<position> TerrainManager::grassPositions;

Terrain::Terrain(float offsetx, float offsety)
  : heightMultiplier(25.0f),
    noiseScale(10.0f),
    resX(RES_X),
    resY(RES_Y)
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
    terrains.push_back(std::make_unique<Terrain>(0.0f, 0.0f));
    terrains.push_back(std::make_unique<Terrain>(0.0f, 50.0f));
    terrains.push_back(std::make_unique<Terrain>(0.0f, 100.0f));
    terrains.push_back(std::make_unique<Terrain>(50.0f, 0.0f));
    terrains.push_back(std::make_unique<Terrain>(50.0f, 50.0f));
    terrains.push_back(std::make_unique<Terrain>(50.0f, 100.0f));
    terrains.push_back(std::make_unique<Terrain>(100.0f, 0.0f));
    terrains.push_back(std::make_unique<Terrain>(100.0f, 50.0f));
    terrains.push_back(std::make_unique<Terrain>(100.0f, 100.0f));
}

void TerrainManager::DrawTerrains()
{
    // rlEnableWireMode();
    DrawModel(terrains[0]->getTerrain(), Vector3Zero(), 1, WHITE);
    DrawModel(terrains[1]->getTerrain(), Vector3{0.0f, 0.0f, 50.0f}, 1, WHITE);
    DrawModel(terrains[2]->getTerrain(), Vector3{0.0f, 0.0f, 100.0f}, 1, WHITE);
    DrawModel(terrains[3]->getTerrain(), Vector3{50.0f, 0.0f, 0.0f}, 1, WHITE);
    DrawModel(terrains[4]->getTerrain(), Vector3{50.0f, 0.0f, 50.0f}, 1, WHITE);
    DrawModel(terrains[5]->getTerrain(), Vector3{50.0f, 0.0f, 100.0f}, 1, WHITE);
    DrawModel(terrains[6]->getTerrain(), Vector3{100.0f, 0.0f, 0.0f}, 1, WHITE);
    DrawModel(terrains[7]->getTerrain(), Vector3{100.0f, 0.0f, 50.0f}, 1, WHITE);
    DrawModel(terrains[8]->getTerrain(), Vector3{100.0f, 0.0f, 100.0f}, 1, WHITE);
    // rlDisableWireMode();
}

Mesh *TerrainManager::getTerrainVertices(int index)
{
    return &terrains[index]->getTerrain().meshes[0];
}