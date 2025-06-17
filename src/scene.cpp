#include "game/scene.hpp"
#include "game/grass.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "game/terrain.hpp"
#include "lights.hpp"
#include "raymath.h"
#include <memory>

std::unordered_map<std::string, std::unique_ptr<Light>> Scene::lights;

void Scene::SetModels()
{
    // Model uia = ResourceManager::getInstance().getModel("oiiaioooooiai");
    // Shader shadowShader = ResourceManager::getInstance().getShader("shadowShader");
    // uia.materials[0].shader = shadowShader;
}

void Scene::SetShaders()
{
    Shader shader = ResourceManager::getInstance().getShader("terrainShader");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);
}

void Scene::SetLights()
{
    Shader shader = ResourceManager::getInstance().getShader("terrainShader");
    lights.emplace("yellow", std::make_unique<Light>(LIGHT_POINT, (Vector3){-9, 7, -9}, Vector3Zero(), YELLOW, shader));
    lights.emplace("red", std::make_unique<Light>(LIGHT_POINT, (Vector3){9, 7, 9}, Vector3Zero(), RED, shader));
    lights.emplace("green", std::make_unique<Light>(LIGHT_POINT, (Vector3){-9, 7, 9}, Vector3Zero(), GREEN, shader));
    lights.emplace("blue", std::make_unique<Light>(LIGHT_POINT, (Vector3){9, 7, -9}, Vector3Zero(), BLUE, shader));
}

void Scene::UpdateShaders()
{
    Vector3 cameraPos = Player::getInstance().camera.position;
    Shader shader = ResourceManager::getInstance().getShader("terrainShader");
    float camera_position[3] = {cameraPos.x, cameraPos.y, cameraPos.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);

    // Check key inputs to enable/disable lights
    if (IsKeyPressed(KEY_Y))
    {
        lights["yellow"]->toggle();
    }
    if (IsKeyPressed(KEY_R))
    {
        lights["red"]->toggle();
    }
    if (IsKeyPressed(KEY_G))
    {
        lights["green"]->toggle();
    }
    if (IsKeyPressed(KEY_B))
    {
        lights["blue"]->toggle();
    }

    for (auto &[_, light] : lights)
        light->UpdateLightShader();
}

void Scene::DrawScene()
{

    BeginShaderMode(ResourceManager::getInstance().getShader("terrainShader"));
    TerrainManager::DrawTerrains();
    DrawCube(Vector3Zero(), 10, 20, 10, GRAY);
    DrawCubeWires(Vector3Zero(), 10, 20, 10, WHITE);
    EndShaderMode();

    for (auto &[_, light] : lights)
    {
        if (light->isEnabled())
        {
            std::cout << "IS BEING DRAWN\n";
            DrawSphereEx(light->getPosition(), 1.0f, 8, 8, light->getColor());
        }
    }
}
