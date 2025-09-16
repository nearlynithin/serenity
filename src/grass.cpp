#include "game/grass.hpp"
#include "game/resource.hpp"
#include "game/shader.hpp"
#include "perlin.hpp"
#include "raylib.h"

void Grass::InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height)
{
    auto rm = ResourceManager::getInstance();
    grass = rm.getModel("grass");
    transforms = (Matrix *)RL_CALLOC(MAX_INSTANCES, sizeof(Matrix));

    const float stepSize = 0.4f;

    int instanceCount = 0;
    for (float z = 0; z < height; z += stepSize)
    {
        for (float x = 0; x < width; x += stepSize)
        {
            float jitterX = ((float)GetRandomValue(-50, 50) / 100.0f) * x;
            float jitterZ = ((float)GetRandomValue(-50, 50) / 100.0f) * z;

            float worldX = x - width / 2.0f + jitterX;
            float worldZ = z - height / 2.0f + jitterZ;

            float scaledX = worldX * noise + offset_x * 10.0f;
            float scaledZ = worldZ * noise + offset_y * 10.0f;

            float h = terrainNoise(scaledX, scaledZ, 200.0f);
            float y = h * heightFactor;

            Vector3 pos = {worldX + (width / 2 + offset_x), y, worldZ + (height / 2 + offset_y)};
            float scaleY = GetRandomValue(5, 8);

            Matrix matScale = MatrixScale(4, scaleY, 4);
            Matrix matTrans = MatrixTranslate(pos.x, pos.y, pos.z);

            transforms[instanceCount++] = MatrixMultiply(matScale, matTrans);
        }
    }
    matInstances = grass.materials[0];
    totalInstances = instanceCount;
}

void Grass::Animate(gfx::Shader *grassShader)
{
    float time = GetTime();
    grassShader->SetShaderValue("time", &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(grassShader->getShader(), grassShader->getUniformLoc("time"), &time, SHADER_UNIFORM_FLOAT);
}

void Grass::DrawGrass(Camera3D &camera, SceneContext *sceneCtx)
{
    float shininess = 12.0f;       // Low shininess for matte grass
    float specularStrength = 0.3f; // Very low specular for grass
    float metallicFactor = 0.0f;   // Non-metallic
    float fogDensity = 0.02f;      // Adjust to taste
    Animate(sceneCtx->grassShader);
    sceneCtx->grassShader->getShader().locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocationAttrib(sceneCtx->grassShader->getShader(), "instanceTransform");
    sceneCtx->grassShader->SetShaderValue("camPos", &camera.position, SHADER_UNIFORM_VEC3);
    sceneCtx->grassShader->SetShaderValue("viewPos", &camera.position, SHADER_UNIFORM_VEC3);
    sceneCtx->grassShader->SetShaderValue("camTarget", &camera.target, SHADER_UNIFORM_VEC3);
    sceneCtx->grassShader->SetShaderValue("ambient", sceneCtx->ambient, SHADER_UNIFORM_VEC4);
    sceneCtx->grassShader->SetShaderValue("shininess", &shininess, SHADER_UNIFORM_FLOAT);
    sceneCtx->grassShader->SetShaderValue("specularStrength", &specularStrength, SHADER_UNIFORM_FLOAT);
    sceneCtx->grassShader->SetShaderValue("metallicFactor", &metallicFactor, SHADER_UNIFORM_FLOAT);
    sceneCtx->grassShader->SetShaderValue("fogDensity", &fogDensity, SHADER_UNIFORM_FLOAT);
    sceneCtx->grassShader->SetShaderValue("renderPass", &sceneCtx->renderPass, SHADER_UNIFORM_INT);
    matInstances.shader = sceneCtx->grassShader->getShader();
    DrawMeshInstanced(grass.meshes[0], matInstances, transforms, totalInstances);
}

void Grass::UnloadGrass()
{
    RL_FREE(transforms);
    UnloadMaterial(matInstances);
    UnloadModel(grass);
}
