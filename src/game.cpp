#include "game/game.hpp"
#include "game/lights.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>

Game::Game()
  : screenWidth(1280),
    screenHeight(720),
    mIsRunning(false),
    time(GetTime())
{
}

void Game::LoadAllTextures()
{
    auto &resourceManager = ResourceManager::getInstance();
    resourceManager.LoadTex("terrainTexture", "assets/ground.png");

    shadowMap = resourceManager.LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);
    for (int param : {RL_TEXTURE_MIN_FILTER, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_T})
    {
        rlTextureParameters(shadowMap.depth.id, param, RL_TEXTURE_FILTER_NEAREST);
    }
}

void Game::LoadAllShaders()
{
    auto &resourceManager = ResourceManager::getInstance();
    resourceManager.Loadshader("terrainLightShader", "assets/shaders/lighting.vs", "assets/shaders/lighting.fs");
    resourceManager.Loadshader("shadowShader", "assets/shaders/shadowmap.vs", "assets/shaders/shadowmap.fs");
}

bool Game::Initialize()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Serenity");
    SetTargetFPS(60);
    DisableCursor();

    LoadAllShaders();
    LoadAllTextures();
    SetShaders();

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 0.0f)); // RED  (0,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 4.5f)); // WHITE (0,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 9.0f)); // BLUE  (0,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 0.0f)); // GREEN (1,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 4.5f)); // YELLOW(1,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 9.0f)); // ORANGE(1,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 0.0f)); // CYAN  (2,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 4.5f)); // PURPLE(2,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 9.0f)); // MAGENTA(2,2)

    SetLights();

    // Setup camera
    camera = {
        .position = {10.0f, 10.0f, 10.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    };

    if (IsWindowReady())
    {
        mIsRunning = true;
    }
    else
    {
        mIsRunning = false;
    }
    return mIsRunning;
}

void Game::Shutdown()
{
    // Destroying resource manager
    ResourceManager::getInstance().UnloadAll();

    if (mIsRunning)
    {
        mIsRunning = false;
        CloseWindow();
    }
}

void Game::RunLoop()
{
    while (mIsRunning && !WindowShouldClose())
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::SetLights()
{
    lightDir = Vector3{0.5f, 0.0f, -0.5f};
    float lightCol[4] = {0.2f, 0.2f, 0.0f, 0.1f};
    Shader shadowShader = ResourceManager::getInstance().getShader("shadowShader");
    SetShaderValue(shadowShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "lightColor"), &lightCol, SHADER_UNIFORM_VEC4);

    float ambient[4] = {0.7f, 0.7f, 0.0f, 0.1f};
    SetShaderValue(shadowShader, GetShaderLocation(shadowShader, "ambient"), ambient, SHADER_UNIFORM_VEC4);
}

void Game::SetShaders()
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    shadow_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shadow_shader, "viewPos");

    lightDirLoc = GetShaderLocation(shadow_shader, "lightDir");
    lightVPLoc = GetShaderLocation(shadow_shader, "lightVP");
    shadowMapLoc = GetShaderLocation(shadow_shader, "shadowMap");

    int shadowMapResolution = SHADOWMAP_RESOLUTION;
    SetShaderValue(shadow_shader, GetShaderLocation(shadow_shader, "shadowMapResolution"), &shadowMapResolution,
                   SHADER_UNIFORM_INT);

    lightCam = {0};
    lightCam.position = Vector3Add(lightDir, {50.0f, 10.0f, 50.0f});
    lightCam.target = Vector3{50.0f, 0.0f, 50.0f};
    lightCam.up = Vector3{0.0f, 1.0f, 0.0f};
    lightCam.fovy = 45.0f;
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
}

void Game::UpdateShaders()
{
    Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    Vector3 cameraPos = camera.position;
    SetShaderValue(shadow_shader, shadow_shader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);

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

    DrawModel(terrains[0]->getTerrain(), Vector3Zero(), 0.1, DARKBROWN);                 // (0,0)
    DrawModel(terrains[1]->getTerrain(), Vector3{0.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);    // (0,1)
    DrawModel(terrains[2]->getTerrain(), Vector3{0.0f, 0.0f, 100.0f}, 0.1, DARKBROWN);   // (0,)
    DrawModel(terrains[3]->getTerrain(), Vector3{50.0f, 0.0f, 0.0f}, 0.1, DARKBROWN);    // (1,0)
    DrawModel(terrains[4]->getTerrain(), Vector3{50.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);   // (1,1)
    DrawModel(terrains[5]->getTerrain(), Vector3{50.0f, 0.0f, 100.0f}, 0.1, DARKBROWN);  // (1,)
    DrawModel(terrains[6]->getTerrain(), Vector3{100.0f, 0.0f, 0.0f}, 0.1, DARKBROWN);   // (2,0)
    DrawModel(terrains[7]->getTerrain(), Vector3{100.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);  // (2,1)
    DrawModel(terrains[8]->getTerrain(), Vector3{100.0f, 0.0f, 100.0f}, 0.1, DARKBROWN); // (2,2)
    DrawCube(Vector3{50.0f, 5.0f, 50.0f}, 15, 15, 15, BROWN);

    EndMode3D();
    EndTextureMode();

    lightViewProj = MatrixMultiply(lightView, lightProj);
    SetShaderValueMatrix(shadow_shader, lightVPLoc, lightViewProj);
    rlEnableShader(shadow_shader.id);
}

void Game::ProcessInput()
{
}

void Game::UpdateGame()
{
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    UpdateShaders();
}

void Game::GenerateOutput()
{
    time += GetFrameTime();

    BeginDrawing();
    ClearBackground(DARKBROWN);

    const int SHADOW_MAP_SLOT = 1;
    rlActiveTextureSlot(SHADOW_MAP_SLOT);
    rlEnableTexture(shadowMap.depth.id);
    rlSetUniform(shadowMapLoc, &SHADOW_MAP_SLOT, SHADER_UNIFORM_INT, 1);

    BeginMode3D(camera);
    BeginShaderMode(ResourceManager::getInstance().getShader("shadowShader"));
    DrawModel(terrains[0]->getTerrain(), Vector3Zero(), 0.1, DARKBROWN);                 // (0,0)
    DrawModel(terrains[1]->getTerrain(), Vector3{0.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);    // (0,1)
    DrawModel(terrains[2]->getTerrain(), Vector3{0.0f, 0.0f, 100.0f}, 0.1, DARKBROWN);   // (0,)
    DrawModel(terrains[3]->getTerrain(), Vector3{50.0f, 0.0f, 0.0f}, 0.1, DARKBROWN);    // (1,0)
    DrawModel(terrains[4]->getTerrain(), Vector3{50.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);   // (1,1)
    DrawModel(terrains[5]->getTerrain(), Vector3{50.0f, 0.0f, 100.0f}, 0.1, DARKBROWN);  // (1,)
    DrawModel(terrains[6]->getTerrain(), Vector3{100.0f, 0.0f, 0.0f}, 0.1, DARKBROWN);   // (2,0)
    DrawModel(terrains[7]->getTerrain(), Vector3{100.0f, 0.0f, 50.0f}, 0.1, DARKBROWN);  // (2,1)
    DrawModel(terrains[8]->getTerrain(), Vector3{100.0f, 0.0f, 100.0f}, 0.1, DARKBROWN); // (2,2)
    DrawSphere(lightCam.position, 1.0f, RED);
    DrawCube(Vector3{50.0f, 5.0f, 50.0f}, 15, 15, 15, BROWN);
    EndShaderMode();
    EndMode3D();

    // DrawTextureRec(shadowMap.depth, {0, 0, (float)shadowMap.depth.width, -(float)shadowMap.depth.height}, {10,
    // 10},WHITE);

    EndDrawing();
}