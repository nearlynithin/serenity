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

void Scene::DrawScene()
{
    // rlEnableWireMode();
    TerrainManager::DrawTerrains();
    // rlDisableWireMode();
    rlDisableBackfaceCulling();
    // Grass::DrawGrass();
    rlEnableBackfaceCulling();
}