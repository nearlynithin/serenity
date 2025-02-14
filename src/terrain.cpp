#include "game/terrain.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>

std::vector<std::unique_ptr<Terrain>> TerrainManager::terrains;

Terrain::Terrain(float width, float length, float offsetx, float offsety)
  : heightMultiplier(70.0f),
    noiseScale(0.009f),
    resX(100),
    resY(100)
{
    mesh = GenMeshPlane(width, length, resX, resY);
    vertexData = (float *)malloc(mesh.vertexCount * 3 * sizeof(float));
    memcpy(vertexData, mesh.vertices, mesh.vertexCount * 3 * sizeof(float));
    terrain = LoadModelFromMesh(mesh);

    for (int i = 0; i < mesh.vertexCount * 3; i += 3)
    {
        float scaledX = (vertexData[i] + 1000.0f) * noiseScale;
        float scaledZ = (vertexData[i + 2] + 1000.0f) * noiseScale;
        vertexData[i + 1] = perlin(scaledX + offsetx, scaledZ + offsety, 1.0f) * heightMultiplier;
    }
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);

    setTexture();
    setShader();
}

void Terrain::updateTerrain(float time, float offsetx, float offsety)
{
    for (int i = 0; i < mesh.vertexCount * 3; i += 3)
    {
        // Add an offset to center the noise pattern
        float scaledX = (vertexData[i] + 1000.0f) * noiseScale;
        float scaledZ = (vertexData[i + 2] + 1000.0f) * noiseScale;
        vertexData[i + 1] = perlin(scaledX + offsetx, scaledZ + offsety + time, 1.0f) * heightMultiplier;
    }
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);
}

Model &Terrain::getTerrain()
{
    return terrain;
}

void Terrain::setTexture()
{
    terrain.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
        ResourceManager::getInstance().GetTexture("terrainTexture");
}

void Terrain::setShader()
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    terrain.materials[0].shader = shadow_shader;
}

Terrain::~Terrain()
{
    free(vertexData);
    // Terrain objects are automatically destroyed because of unique_ptr
}

// Terrain manager
void TerrainManager::LoadTerrains()
{
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 0.0f)); // RED  (0,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 4.5f)); // WHITE (0,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 9.0f)); // BLUE  (0,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 0.0f)); // GREEN (1,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 4.5f)); // YELLOW(1,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 9.0f)); // ORANGE(1,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 0.0f)); // CYAN  (2,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 4.5f)); // PURPLE(2,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 9.0f)); // MAGENTA(2,2)
}

void TerrainManager::DrawTerrains()
{
    DrawModel(terrains[0]->getTerrain(), Vector3Zero(), 1, DARKBROWN);                   // (0,0)
    DrawModel(terrains[1]->getTerrain(), Vector3{0.0f, 0.0f, 500.0f}, 1, DARKBROWN);     // (0,1)
    DrawModel(terrains[2]->getTerrain(), Vector3{0.0f, 0.0f, 1000.0f}, 1, DARKBROWN);    // (0,)
    DrawModel(terrains[3]->getTerrain(), Vector3{500.0f, 0.0f, 0.0f}, 1, DARKBROWN);     // (1,0)
    DrawModel(terrains[4]->getTerrain(), Vector3{500.0f, 0.0f, 500.0f}, 1, DARKBROWN);   // (1,1)
    DrawModel(terrains[5]->getTerrain(), Vector3{500.0f, 0.0f, 1000.0f}, 1, DARKBROWN);  // (1,)
    DrawModel(terrains[6]->getTerrain(), Vector3{1000.0f, 0.0f, 0.0f}, 1, DARKBROWN);    // (2,0)
    DrawModel(terrains[7]->getTerrain(), Vector3{1000.0f, 0.0f, 500.0f}, 1, DARKBROWN);  // (2,1)
    DrawModel(terrains[8]->getTerrain(), Vector3{1000.0f, 0.0f, 1000.0f}, 1, DARKBROWN); // (2,2)
}
