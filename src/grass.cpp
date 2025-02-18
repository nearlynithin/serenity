#include "game/grass.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raylib.h"
#include <iostream>

Model Grass::grass = {0};
Matrix *Grass::transforms = nullptr;
Material Grass::matInstances = {0};
long Grass::count = -1;

void Grass::InitGrass()
{
    grass = LoadModel("assets/grass.obj");
    transforms = (Matrix *)RL_CALLOC(MAX_INSTANCES, sizeof(Matrix));

    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        position pos = getGrassPosition();
        Matrix translation = MatrixTranslate(pos.x, pos.y, pos.z);
        Vector3 axis = {0.0f, 1.0f, 0.0f}; // Fixed, rotating around Y-axis only
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        Matrix rotation = MatrixRotate(axis, angle);
        Matrix scaled = MatrixMultiply(MatrixScale(40, GetRandomValue(10, 40), 40), rotation);
        transforms[i] = MatrixMultiply(scaled, translation);
    }

    Shader shader = ResourceManager::getInstance().getShader("grassShader");
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    matInstances = grass.materials[0];
    matInstances.shader = shader;
}

position Grass::getGrassPosition()
{
    count++;

    // Return original terrain points for the first 10200 positions
    if (count <= 10200)
    {
        return TerrainManager::grassPositions[count];
    }

    // Use points that are further apart to prevent overlapping
    const int spacing = 20; // Adjust this value based on your grass blade size
    int baseIndex = (count % (10200 - spacing * 2));

    // Get three points with sufficient spacing between them
    position p1 = TerrainManager::grassPositions[baseIndex];
    position p2 = TerrainManager::grassPositions[baseIndex + spacing];
    position p3 = TerrainManager::grassPositions[baseIndex + spacing * 2];

    // Generate barycentric coordinates
    float u = GetRandomValue(0.0f, 1.0f);
    float v = GetRandomValue(0.0f, 1.0f - u);
    float w = 1.0f - u - v;

    // Interpolate using barycentric coordinates
    position result;
    result.x = u * p1.x + v * p2.x + w * p3.x;
    result.y = u * p1.y + v * p2.y + w * p3.y;
    result.z = u * p1.z + v * p2.z + w * p3.z;

    // Add proportional jitter based on the spacing
    float jitter = spacing * 0.1f; // Jitter is now proportional to spacing
    result.x += GetRandomValue(-jitter, jitter);
    result.z += GetRandomValue(-jitter, jitter);

    return result;
}

void Grass::DrawGrass()
{
    // DrawModel(grass, Vector3{10.0f, 0.0f, 10.0f}, 50.0f, BLUE);
    DrawMeshInstanced(grass.meshes[0], matInstances, transforms, MAX_INSTANCES);
}

void Grass::UnloadGrass()
{
    RL_FREE(transforms);
    UnloadMaterial(matInstances);
    UnloadModel(grass);
}