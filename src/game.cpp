#include "game/game.hpp"
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

bool Game::Initialize()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // Multi-Sample Anti-Aliasing (MSAA) with 4x sampling.
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(screenWidth, screenHeight, "Serenity");

    SetTargetFPS(60);
    DisableCursor();

    // LoadTextures
    LoadAllTextures();

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 0.0f)); // RED  (0,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 4.5f)); // WHITE (0,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 0.0f, 9.0f)); // BLUE  (0,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 0.0f)); // GREEN (1,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 4.5f)); // YELLOW(1,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 4.5f, 9.0f)); // ORANGE(1,2)

    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 0.0f)); // CYAN  (2,0)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 4.5f)); // PURPLE(2,1)
    terrains.push_back(std::make_unique<Terrain>(500.0f, 500.0f, 9.0f, 9.0f)); // MAGENTA(2,2)

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

void Game::LoadAllTextures()
{
    auto &resourceManager = ResourceManager::getInstance();
    resourceManager.LoadTex("terrainTexture", "assets/ground.png");
}

void Game::ProcessInput()
{
}

void Game::UpdateGame()
{
    // std::cout << "update game\n";
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
}

void Game::GenerateOutput()
{
    time += GetFrameTime();

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);

    rlPushMatrix();
    // rlEnableWireMode();

    DrawModel(terrains[0]->getTerrain(), Vector3Zero(), 0.1, WHITE);                 // (0,0)
    DrawModel(terrains[1]->getTerrain(), Vector3{0.0f, 0.0f, 50.0f}, 0.1, WHITE);    // (0,1)
    DrawModel(terrains[2]->getTerrain(), Vector3{0.0f, 0.0f, 100.0f}, 0.1, WHITE);   // (0,)
    DrawModel(terrains[3]->getTerrain(), Vector3{50.0f, 0.0f, 0.0f}, 0.1, WHITE);    // (1,0)
    DrawModel(terrains[4]->getTerrain(), Vector3{50.0f, 0.0f, 50.0f}, 0.1, WHITE);   // (1,1)
    DrawModel(terrains[5]->getTerrain(), Vector3{50.0f, 0.0f, 100.0f}, 0.1, WHITE);  // (1,)
    DrawModel(terrains[6]->getTerrain(), Vector3{100.0f, 0.0f, 0.0f}, 0.1, WHITE);   // (2,0)
    DrawModel(terrains[7]->getTerrain(), Vector3{100.0f, 0.0f, 50.0f}, 0.1, WHITE);  // (2,1)
    DrawModel(terrains[8]->getTerrain(), Vector3{100.0f, 0.0f, 100.0f}, 0.1, WHITE); // (2,2)

    // rlDisableWireMode();
    rlPopMatrix();
    EndMode3D();

    EndDrawing();
}