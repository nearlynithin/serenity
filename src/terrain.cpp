#include "game/terrain.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include <stdlib.h>
#include <string.h>

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

Terrain::~Terrain()
{
    free(vertexData);
    // Terrain objects are automatically destroyed because of unique_ptr
}