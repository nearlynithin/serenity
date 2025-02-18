#include "game/game.hpp"
#include "game/grass.hpp"
#include "game/lights.hpp"
#include "game/resource.hpp"
#include "game/scene.hpp"
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Serenity");
    SetTargetFPS(60);
    DisableCursor();

    ResourceLoader::LoadAllTextures();
    ResourceLoader::LoadAllShaders();
    Scene::getInstance().SetShaders();
    TerrainManager::LoadTerrains();
    Scene::getInstance().SetLights();
    Grass::InitGrass();

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

void Game::ProcessInput()
{
}

void Game::UpdateGame()
{
    // UpdateCameraPro(&camera,
    //                 (Vector3){
    //                     (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * 1.0f - // Move forward-backward
    //                         (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * 1.0f,
    //                     (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * 1.0f - // Move right-left
    //                         (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * 1.0f,
    //                     0.0f // Move up-down
    //                 },
    //                 (Vector3){
    //                     // Edit delta X for faster sideways movement
    //                     GetMouseDelta().x * 0.1f,  // Rotation: yaw
    //                     GetMouseDelta().y * 0.08f, // Rotation: pitch
    //                     0.0f                       // Rotation: roll
    //                 },
    //                 0.0f); // Move to target (zoom)
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    Scene::getInstance().UpdateShaders(&camera);
}

void Game::GenerateOutput()
{
    time += GetFrameTime();

    BeginDrawing();
    Color backg = ColorFromNormalized(Vector4{0.8, 1.0, 0.8, 1.0});
    ClearBackground(backg);

    Scene::getInstance().ScenePrep();

    BeginMode3D(camera);
    Scene::getInstance().DrawScene();
    EndMode3D();

    DrawFPS(20, 20);

    // DrawTextureRec(shadowMap.depth, {0, 0, (float)shadowMap.depth.width, -(float)shadowMap.depth.height}, {10,
    // 10},WHITE);

    EndDrawing();
}