#include "game/terrain.hpp"
#include "game/grass.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raymath.h"
#include "utils.hpp"
#include <stdlib.h>
#include <string.h>
#include <unordered_set>

std::unordered_map<Vector2, std::unique_ptr<Terrain>> TerrainManager::terrains;
Vector2 TerrainManager::currentTerrain;
std::unordered_set<Vector2> TerrainManager::cords;
RayCollision TerrainManager::collision;
Ray TerrainManager::playerMarker;
Grass TerrainManager::grass;

Terrain::Terrain(float offsetx, float offsety)
  : resX(RES_X),
    resY(RES_Y),
    position(Vector3{WIDTH / 2 + offsetx, 0, HEIGHT / 2 + offsety})
{
    mesh = GenMeshPlane(WIDTH, HEIGHT, resX, resY);
    vertexData = (float *)malloc(mesh.vertexCount * 3 * sizeof(float));
    memcpy(vertexData, mesh.vertices, mesh.vertexCount * 3 * sizeof(float));

    for (int i = 0; i < mesh.vertexCount * 3; i += 3)
    {
        float scaledX = vertexData[i] * NOISE_SCALE;
        float scaledZ = vertexData[i + 2] * NOISE_SCALE;

        float height = terrainNoise(scaledX + offsetx * 10, scaledZ + offsety * 10, 100.0f);
        vertexData[i + 1] = height * HEIGHT_FACTOR;
    }
    memcpy(mesh.vertices, vertexData, mesh.vertexCount * 3 * sizeof(float));
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);
    terrain = LoadModelFromMesh(mesh);
    bbox = GetMeshBoundingBox(mesh);
    bbox.min = Vector3Add(bbox.min, position);
    bbox.max = Vector3Add(bbox.max, position);
    terrain.transform = MatrixTranslate(position.x, position.y, position.z);
    grass.InitGrass(offsetx, offsety, NOISE_SCALE, HEIGHT_FACTOR, WIDTH, HEIGHT, terrain.meshes[0]);
    setTexture();
    setShader();
}
Model &Terrain::getTerrain()
{
    return terrain;
}
Vector3 Terrain::getPosition()
{
    return position;
}

void Terrain::setTexture()
{
    Texture2D tex = ResourceManager::getInstance().GetTexture("terrainTexture");
    terrain.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
    GenTextureMipmaps(&tex);
    SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
}

void Terrain::setShader()
{
    // Shader shadow_shader = ResourceManager::getInstance().getShader("shadowShader");
    // terrain.materials[0].shader = shadow_shader;
}

BoundingBox &Terrain::getBBox()
{
    return bbox;
}

void Terrain::DrawGrass()
{
    grass.DrawGrass();
}

Terrain::~Terrain()
{
    free(vertexData);
    UnloadModel(terrain);
}

// Terrain manager
void TerrainManager::LoadTerrains()
{
    for (int i = -MAX_TERRAIN / 2; i < MAX_TERRAIN / 2; i++)
    {
        for (int j = -MAX_TERRAIN; j < MAX_TERRAIN / 2; j++)
        {
            Vector2 pos = Vector2{static_cast<float>(i), static_cast<float>(j)};
            if (terrains.count(pos) == 0)
            {
                terrains[pos] = std::make_unique<Terrain>(i * WIDTH, j * HEIGHT);
            }
        }
    }
    currentTerrain = Vector2{0, 0};
    Vector3 markerpos = Vector3Add(Vector3{0.0f, 100.0f, 0.0f}, Player::getInstance().GetPlayerPosition());
    playerMarker = Ray{markerpos, Vector3{0.0f, -1.0f, 0.0f}};
    updateCords(currentTerrain);
}

void TerrainManager::DrawTerrains()
{
    for (auto &cord : cords)
    {
        Vector2 pos = Vector2{cord.x, cord.y};
        // the terrain position is translated into the terrain model in the constructor :)
        DrawModel(terrains[pos]->getTerrain(), Vector3Zero(), 1, DARKBROWN);
        rlDisableBackfaceCulling();
        terrains[pos]->DrawGrass();
        rlEnableBackfaceCulling();
    }
}

void TerrainManager::UpdateTerrains()
{
    Player &player = Player::getInstance();
    Vector3 pos = player.GetPlayerPosition();
    float gridX = static_cast<int>(floorf((pos.x + WIDTH * 5.0f) / WIDTH) - 5);
    float gridZ = static_cast<int>(floorf((pos.z + HEIGHT * 5.0f) / HEIGHT) - 5);
    Vector2 c_pos = Vector2{gridX, gridZ};
    if (c_pos != currentTerrain)
    {
        updateCords(c_pos);
    }
    currentTerrain = c_pos;
}
void TerrainManager::DrawTerrainGrid()
{
    DrawGrid(10, 50);
}

void TerrainManager::updateCords(Vector2 &pos)
{
    cords.clear();
    cords.insert(pos);
    cords.insert(Vector2{pos.x, pos.y + 1});
    cords.insert(Vector2{pos.x, pos.y - 1});
    cords.insert(Vector2{pos.x - 1, pos.y});
    cords.insert(Vector2{pos.x + 1, pos.y});
    cords.insert(Vector2{pos.x - 1, pos.y + 1});
    cords.insert(Vector2{pos.x + 1, pos.y - 1});
    cords.insert(Vector2{pos.x + 1, pos.y + 1});
    cords.insert(Vector2{pos.x - 1, pos.y - 1});
}

void TerrainManager::UpdateCollision()
{
    Player player = Player::getInstance();
    Vector3 markerOrigin = Vector3Add(player.GetPlayerPosition(), Vector3{0.0f, 100.0f, 0.0f});
    playerMarker = Ray{markerOrigin, Vector3{0.0f, -1.0f, 0.0f}};
    for (auto it = terrains.begin(); it != terrains.end();)
    {
        Terrain &terrain = *it->second;
        RayCollision boxHitInfo = GetRayCollisionBox(playerMarker, terrain.getBBox());

        if ((boxHitInfo.hit) && (boxHitInfo.distance < __FLT_MAX__))
        {
            RayCollision meshHitInfo;
            meshHitInfo =
                GetRayCollisionMesh(playerMarker, terrain.getTerrain().meshes[0], terrain.getTerrain().transform);
            if (meshHitInfo.hit)
            {
                Vector3 playerpos = player.GetPlayerPosition();
                playerpos = Vector3{playerpos.x, meshHitInfo.point.y, playerpos.z};
                Player::getInstance().setPlayerPosition(playerpos);
                playerMarker.position.x = playerpos.x;
                playerMarker.position.z = playerpos.z;
                break;
            }
        }
        it++;
    }
}