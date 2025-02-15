#include "game/scene.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "raymath.h"

void Scene::SetLights()
{
    lightDir = Vector3{0.5f, 0.0f, -0.5f};
    float lightCol[4] = {0.2f, 0.2f, 0.0f, 0.1f};
    Shader shadowShader = ResourceManager::getInstance().getShader("shadowShader");
    SetShaderValue(shadowShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "lightColor"), &lightCol, SHADER_UNIFORM_VEC4);

    float ambient[4] = {0.7f, 0.7f, 0.0f, 0.1f};
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "ambient"), ambient, SHADER_UNIFORM_VEC4);
}

void Scene::SetShaders()
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    shadow_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shadow_shader, "viewPos");

    lightDirLoc = GetShaderLocation(shadow_shader, "lightDir");
    lightVPLoc = GetShaderLocation(shadow_shader, "lightVP");
    shadowMapLoc = GetShaderLocation(shadow_shader, "shadowMap");
    fogDensityLoc = GetShaderLocation(shadow_shader, "fogDensity");
    int shadowMapResolution = SHADOWMAP_RESOLUTION;
    SetShaderValue(shadow_shader, GetShaderLocation(shadow_shader, "shadowMapResolution"), &shadowMapResolution,
                   SHADER_UNIFORM_INT);
    fogDensity = 0.15f;

    lightCam = {0};
    lightCam.position = Vector3Add(lightDir, {50.0f, 10.0f, 50.0f});
    lightCam.target = Vector3{50.0f, 0.0f, 50.0f};
    lightCam.up = Vector3{0.0f, 1.0f, 0.0f};
    lightCam.fovy = 45.0f;
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
}

void Scene::UpdateShaders(Camera3D *camera)
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    RenderTexture2D shadowMap = ResourceLoader::getShadowMap();
    Vector3 cameraPos = camera->position;

    SetShaderValue(shadow_shader, shadow_shader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(shadow_shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    float t = GetTime();
    lightDir = Vector3Normalize(Vector3{cosf(t) * 0.5f, -2.0f, sinf(t) * 0.5f});
    lightCam.position = Vector3Scale(lightDir, -50.0f);
    lightCam.target = Vector3{50.0f, 0.0f, 50.0f};

    SetShaderValue(shadow_shader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);

    BeginTextureMode(shadowMap);
    ClearBackground(WHITE);
    BeginMode3D(lightCam);

    lightView = rlGetMatrixModelview();
    lightProj = rlGetMatrixProjection();

    TerrainManager::DrawTerrains();
    // DrawCube(Vector3{50.0f, 5.0f, 50.0f}, 15, 15, 15, BROWN);
    DrawCapsule(Vector3{20.0f, 0.0f, 20.0f}, Vector3{20.0f, 5.0f, 20.0f}, 2, 50, 3, RED);

    EndMode3D();
    EndTextureMode();

    lightViewProj = MatrixMultiply(lightView, lightProj);
    SetShaderValueMatrix(shadow_shader, lightVPLoc, lightViewProj);
    rlEnableShader(shadow_shader.id);
}

void Scene::ScenePrep()
{
    RenderTexture2D shadowMap = ResourceLoader::getShadowMap();
    rlActiveTextureSlot(SHADOW_MAP_SLOT);
    const int slot = SHADOW_MAP_SLOT;
    rlEnableTexture(shadowMap.depth.id);
    rlSetUniform(shadowMapLoc, &slot, SHADER_UNIFORM_INT, 1);
}

void Scene::DrawScene()
{
    BeginShaderMode(ResourceManager::getInstance().getShader("shadowShader"));
    TerrainManager::DrawTerrains();
    DrawSphere(lightCam.position, 1.0f, RED);
    EndShaderMode();
    // DrawCapsule(Vector3{20.0f, 0.0f, 20.0f}, Vector3{20.0f, 5.0f, 20.0f}, 2, 50, 3, RED);
}