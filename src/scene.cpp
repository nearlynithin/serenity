#include "game/scene.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "raymath.h"

void Scene::ScenePrep()
{
    float ambience[4] = {0.16f, 0.109f, 0.09f, 0.5f};
    std::copy(ambience, ambience + 4, sceneCtx.ambient);
    sceneCtx.lightCam = Camera3D{0};
    sceneCtx.lightDir = Vector3Normalize(Vector3{0.0f, -1.0f, 0.0});
    sceneCtx.lightCam.position = Vector3{0.0f, 30.0f, 0.0f};
    sceneCtx.lightCam.target = Vector3Zero();
    sceneCtx.lightCam.projection = CAMERA_ORTHOGRAPHIC;
    sceneCtx.lightCam.up = Vector3{0.0f, 1.0f, 0.0f};
    sceneCtx.lightCam.fovy = 75.0f; // Increased FOV for better coverage
    sceneCtx.lightColor = ColorNormalize(YELLOW);
}

void Scene::SetShaders()
{
    // Terrain Shaders and Grass shaders
    TerrainManager::setShaders(&sceneCtx);

    // Skybox Shaders
    auto rm = ResourceManager::getInstance();
    Model skybox = rm.getModel("skybox");
    sceneCtx.skyboxShader = rm.getShader("skyboxShader");
    sceneCtx.cubemapShader = rm.getShader("cubemapShader");
    Image img = LoadImage("assets/skybox.png");
    const int mtrCubemap = MATERIAL_MAP_CUBEMAP;
    const int userHdr = 0;
    const int eqRect = 0;
    sceneCtx.skyboxShader->addUniform("environmentMap");
    sceneCtx.skyboxShader->addUniform("doGamma");
    sceneCtx.skyboxShader->addUniform("vflipped");
    sceneCtx.skyboxShader->addUniform("equirectangularMap");
    sceneCtx.skyboxShader->SetShaderValue("environmentMap", &mtrCubemap, SHADER_UNIFORM_INT);
    sceneCtx.skyboxShader->SetShaderValue("doGamma", &userHdr, SHADER_UNIFORM_INT);
    sceneCtx.skyboxShader->SetShaderValue("vflipped", &userHdr, SHADER_UNIFORM_INT);
    sceneCtx.skyboxShader->SetShaderValue("equirectangularMap", &eqRect, SHADER_UNIFORM_INT);
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    skybox.materials[0].shader = sceneCtx.skyboxShader->getShader();
    UnloadImage(img);

    // init Shadow mapping shaders
    InitShadowMapping();
}

void Scene::SetLights()
{
}

void Scene::UpdateShaders()
{
    float dt = GetFrameTime();
    Vector3 cameraPos = Player::getInstance().camera.position;
    float camera_position[3] = {cameraPos.x, cameraPos.y, cameraPos.z};
    SetShaderValue(sceneCtx.terrainShader->getShader(), sceneCtx.terrainShader->getUniformLoc("viewPos"),
                   camera_position, SHADER_UNIFORM_VEC3);

    float lightDir[3] = {sceneCtx.lightDir.x, sceneCtx.lightDir.y, sceneCtx.lightDir.z};
    sceneCtx.terrainShader->SetShaderValue("lightDir", &lightDir, SHADER_UNIFORM_VEC3);
    float lightColor[4] = {sceneCtx.lightColor.w, sceneCtx.lightColor.x, sceneCtx.lightColor.y, sceneCtx.lightColor.z};
    sceneCtx.terrainShader->SetShaderValue("lightColor", &lightColor, SHADER_UNIFORM_VEC4);
    sceneCtx.terrainShader->SetShaderValue("ambient", &sceneCtx.ambient, SHADER_UNIFORM_VEC4);
    SetShaderValueMatrix(sceneCtx.terrainShader->getShader(), sceneCtx.terrainShader->getUniformLoc("lightVP"),
                         shadowData.lightViewProj);
    SetShaderValueMatrix(sceneCtx.grassShader->getShader(), sceneCtx.grassShader->getUniformLoc("lightVP"),
                         shadowData.lightViewProj);
    const float cameraSpeed = 25.00f;
    // Define a position for the light in world space
    if (IsKeyDown(KEY_LEFT))
        sceneCtx.lightCam.position.x -= cameraSpeed * dt;
    if (IsKeyDown(KEY_RIGHT))
        sceneCtx.lightCam.position.x += cameraSpeed * dt;
    if (IsKeyDown(KEY_UP))
        sceneCtx.lightCam.position.z -= cameraSpeed * dt;
    if (IsKeyDown(KEY_DOWN))
        sceneCtx.lightCam.position.z += cameraSpeed * dt;
    if (IsKeyDown(KEY_O))
        sceneCtx.lightCam.position.y += cameraSpeed * dt;
    if (IsKeyDown(KEY_P))
        sceneCtx.lightCam.position.y -= cameraSpeed * dt;

    // Always point towards the scene center
    sceneCtx.lightCam.target = Vector3Zero();
    // Update light direction for the shader (normalized from pos to target)
    sceneCtx.lightDir = Vector3Normalize(Vector3Subtract(sceneCtx.lightCam.target, sceneCtx.lightCam.position));
}

void Scene::InitShadowMapping()
{
    auto rm = ResourceManager::getInstance();
    shadowData.shadowMap = rm.LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
    sceneCtx.shadowMapShader = rm.getShader("shadowMapShader");
    sceneCtx.shadowMapShader->addUniform("mvp");
    sceneCtx.shadowMapShader->addUniform("matModel");
    sceneCtx.shadowMapShader->addUniform("matNormal");

    // setup terrain shader for shadow mapping
    sceneCtx.terrainShader->addUniform("shadowMap");
    sceneCtx.terrainShader->addUniform("shadowMapResolution");
    sceneCtx.terrainShader->addUniform("lightVP");
    // sceneCtx.terrainShader->SetShaderValue("shadowMap", &shadowData.shadowMapSlot, SHADER_UNIFORM_INT);
    float shadowRes = SHADOWMAP_RESOLUTION;
    sceneCtx.terrainShader->SetShaderValue("shadowMapResolution", &shadowRes, SHADER_UNIFORM_INT);
    // setup grass Shader for shadow mapping
    sceneCtx.grassShader->addUniform("shadowMap");
    sceneCtx.grassShader->addUniform("shadowMapResolution");
    sceneCtx.grassShader->addUniform("lightVP");
    sceneCtx.grassShader->SetShaderValue("shadowMapResolution", &shadowRes, SHADER_UNIFORM_INT);
}

void Scene::RenderShadowMap()
{
    BeginTextureMode(shadowData.shadowMap);
    ClearBackground(WHITE);
    BeginMode3D(sceneCtx.lightCam);
    shadowData.lightView = rlGetMatrixModelview();
    shadowData.lightProj = rlGetMatrixProjection();
    TerrainManager::DrawTerrains(&sceneCtx, true);
    DrawCube(Vector3Zero(), 10, 40, 10, RAYWHITE);
    EndMode3D();
    EndTextureMode();

    shadowData.lightViewProj = MatrixMultiply(shadowData.lightView, shadowData.lightProj);
}

void Scene::DrawScene()
{
    BeginDrawing();
    RenderShadowMap();

    auto &rm = ResourceManager::getInstance();
    ClearBackground(BLACK);
    rlEnableShader(sceneCtx.terrainShader->getShader().id);
    rlActiveTextureSlot(shadowData.shadowMapSlot);
    rlEnableTexture(shadowData.shadowMap.depth.id);
    rlSetUniform(sceneCtx.terrainShader->getUniformLoc("shadowMap"), &shadowData.shadowMapSlot, SHADER_UNIFORM_INT, 1);
    rlSetUniform(sceneCtx.grassShader->getUniformLoc("shadowMap"), &shadowData.shadowMapSlot, SHADER_UNIFORM_INT, 1);

    ClearBackground(BLACK);
    BeginMode3D(Player::camera);

    // draw skybox
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(rm.getModel("skybox"), Vector3{0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();

    rlEnableShader(sceneCtx.terrainShader->getShader().id);
    rlActiveTextureSlot(shadowData.shadowMapSlot);
    rlEnableTexture(shadowData.shadowMap.depth.id);
    TerrainManager::DrawTerrains(&sceneCtx, false);
    BeginShaderMode(sceneCtx.terrainShader->getShader());
    DrawCube(Vector3Zero(), 10, 40, 10, RAYWHITE);
    EndShaderMode();
    // DrawSphere(sceneCtx.lightCam.position, 2.0f, WHITE);
    // Vector3 lightEnd = Vector3Add(Vector3Zero(), Vector3Scale(sceneCtx.lightDir, 70.0f));
    // DrawLine3D(sceneCtx.lightCam.position, lightEnd, RED);

    Player::DrawPlayer();

    EndMode3D();
    EndDrawing();
}
