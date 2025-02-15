#include "game/grass.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raylib.h"
#include <iostream>

Model Grass::grass = {0};
Matrix *Grass::transforms = nullptr;
Material Grass::matInstances = {0};

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
        Matrix scaled = MatrixMultiply(MatrixScale(10, 10, 10), rotation);
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
    // matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;
    // assign the instancing shader here
}

position Grass::getGrassPosition()
{
    float x = GetRandomValue(-50, 50);
    float z = GetRandomValue(-50, 50);
    float y = perlin(x, z, 1.0f) * 70.0f;
    return {x, y, z};
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