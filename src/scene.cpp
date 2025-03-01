#include "game/scene.hpp"
#include "game/grass.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "raymath.h"

void Scene::SetModels()
{
    Model uia = ResourceManager::getInstance().getModel("oiiaioooooiai");
    Shader shadowShader = ResourceManager::getInstance().getShader("shadowShader");
    uia.materials[0].shader = shadowShader;
}

void Scene::SetLights()
{
    lightDir = Vector3{0.5f, 0.0f, -0.5f};
    float lightCol[4] = {0.2f, 0.2f, 0.0f, 0.1f};
    float ambient[4] = {0.7f, 0.7f, 0.0f, 0.1f};

    // Set values for both shaders
    Shader shadowShader = ResourceManager::getInstance().getShader("shadowShader");
    Shader grassShader = ResourceManager::getInstance().getShader("grassShader");

    // Set light direction
    SetShaderValue(shadowShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(grassShader, GetShaderLocation(grassShader, "lightDir"), &lightDir, SHADER_UNIFORM_VEC3);

    // Set light color
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "lightColor"), &lightCol, SHADER_UNIFORM_VEC4);
    SetShaderValue(grassShader, GetShaderLocation(grassShader, "lightColor"), &lightCol, SHADER_UNIFORM_VEC4);

    // Set ambient light
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "ambient"), ambient, SHADER_UNIFORM_VEC4);
    SetShaderValue(grassShader, GetShaderLocation(grassShader, "ambient"), ambient, SHADER_UNIFORM_VEC4);
}

void Scene::SetShaders()
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    Shader grass_shader = ResourceManager::getInstance().getShader("grassShader");

    // Shadow shader locations
    shadow_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shadow_shader, "viewPos");
    grass_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(grass_shader, "viewPos");

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
    Shader grass_shader = ResourceManager::getInstance().getShader("grassShader");
    RenderTexture2D shadowMap = ResourceLoader::getShadowMap();
    Vector3 cameraPos = camera->position;

    SetShaderValue(shadow_shader, shadow_shader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(grass_shader, grass_shader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);

    SetShaderValue(shadow_shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(grass_shader, GetShaderLocation(grass_shader, "fogDensity"), &fogDensity, SHADER_UNIFORM_FLOAT);

    float t = GetTime();
    lightDir = Vector3Normalize(Vector3{cosf(t) * 0.5f, -2.0f, sinf(t) * 0.5f});
    lightCam.position = Vector3Scale(lightDir, -50.0f);
    lightCam.target = Vector3{50.0f, 0.0f, 50.0f};

    SetShaderValue(shadow_shader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(grass_shader, GetShaderLocation(grass_shader, "lightDir"), &lightDir, SHADER_UNIFORM_VEC3);

    BeginTextureMode(shadowMap);
    ClearBackground(WHITE);
    BeginMode3D(lightCam);

    lightView = rlGetMatrixModelview();
    lightProj = rlGetMatrixProjection();

    TerrainManager::DrawTerrains();
    DrawModel(ResourceManager::getInstance().getModel("oiiaioooooiai"), Vector3{-20, 10, -20}, 0.2, WHITE);
    Grass::DrawGrass();

    EndMode3D();
    EndTextureMode();

    lightViewProj = MatrixMultiply(lightView, lightProj);

    SetShaderValueMatrix(shadow_shader, lightVPLoc, lightViewProj);
    SetShaderValueMatrix(grass_shader, GetShaderLocation(grass_shader, "lightVP"), lightViewProj);

    SetShaderValueTexture(shadow_shader, shadowMapLoc, shadowMap.texture);
    SetShaderValueTexture(grass_shader, GetShaderLocation(grass_shader, "shadowMap"), shadowMap.texture);

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
    // rlEnableWireMode();
    BeginShaderMode(ResourceManager::getInstance().getShader("shadowShader"));
    TerrainManager::DrawTerrains();
    // rlDisableWireMode();
    DrawSphere(lightCam.position, 1.0f, RED);
    rlDisableBackfaceCulling();
    Grass::DrawGrass();
    rlEnableBackfaceCulling();
    DrawModel(ResourceManager::getInstance().getModel("oiiaioooooiai"), Vector3{0, 7, -20}, 0.8, WHITE);
    EndShaderMode();
    // DrawCapsule(Vector3{20.0f, 0.0f, 20.0f}, Vector3{20.0f, 5.0f, 20.0f}, 2, 50, 3, RED);
}