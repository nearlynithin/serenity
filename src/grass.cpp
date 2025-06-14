#include "game/grass.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raylib.h"
#include <iostream>
#include <string.h>

void Grass::InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height,
                      Mesh &mesh)
{
    grass = ResourceManager::getInstance().getModel("grass");
    transforms = (Matrix *)RL_CALLOC(MAX_INSTANCES, sizeof(Matrix));

    const int gridSize = 200;
    const float stepX = width / (float)(gridSize - 1);
    const float stepZ = height / (float)(gridSize - 1);

    int instanceCount = 0;
    for (int z = 0; z < gridSize && instanceCount < MAX_INSTANCES; z++)
    {
        for (int x = 0; x < gridSize && instanceCount < MAX_INSTANCES; x++)
        {
            if (((float)GetRandomValue(0, 100) / 100.0f) > 0.75f)
                continue;

            float jitterX = ((float)GetRandomValue(-50, 50) / 100.0f) * stepX;
            float jitterZ = ((float)GetRandomValue(-50, 50) / 100.0f) * stepZ;

            float worldX = x * stepX - width / 2.0f + jitterX;
            float worldZ = z * stepZ - height / 2.0f + jitterZ;

            float scaledX = worldX * noise + offset_x * 10.0f;
            float scaledZ = worldZ * noise + offset_y * 10.0f;

            float h = terrainNoise(scaledX, scaledZ, 200.0f);
            float y = h * heightFactor;

            Vector3 pos = {worldX + (width / 2 + offset_x), y, worldZ + (height / 2 + offset_y)};

            float rotX = (float)GetRandomValue(-80, -45) * DEG2RAD;
            float scaleY = GetRandomValue(3, 5);

            Matrix matRot = MatrixRotateX(rotX);
            Matrix matScale = MatrixScale(4, scaleY, 4);
            Matrix matTrans = MatrixTranslate(pos.x, pos.y, pos.z);

            // Final transform: Scale → Rotate → Translate
            Matrix mat = MatrixMultiply(matRot, matScale);
            transforms[instanceCount++] = MatrixMultiply(mat, matTrans);
        }
    }

    Shader shader = ResourceManager::getInstance().getShader("grassShader");
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");
    timeLoc = GetShaderLocation(shader, "time");
    camPosLoc = GetShaderLocation(shader, "cameraPos");
    camTargetLoc = GetShaderLocation(shader, "cameraTarget");
    fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    ambientLoc = GetShaderLocation(shader, "ambient");
    matInstances = grass.materials[0];
    matInstances.shader = shader;
}

void Grass::Animate()
{
    Shader grassShader = ResourceManager::getInstance().getShader("grassShader");
    float time = GetTime();
    SetShaderValue(grassShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
}

void Grass::DrawGrass(Camera3D &camera)
{
    Animate();
    Shader grassShader = ResourceManager::getInstance().getShader("grassShader");
    SetShaderValue(grassShader, camPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(grassShader, grassShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(grassShader, camTargetLoc, &camera.target, SHADER_UNIFORM_VEC3);
    SetShaderValue(grassShader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);
    SetShaderValue(grassShader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    DrawMeshInstanced(grass.meshes[0], matInstances, transforms, MAX_INSTANCES);
}

void Grass::UnloadGrass()
{
    RL_FREE(transforms);
    UnloadMaterial(matInstances);
    UnloadModel(grass);
}