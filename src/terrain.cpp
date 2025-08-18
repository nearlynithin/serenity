#include "game/terrain.hpp"
#include "game/grass.hpp"
#include "game/player.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raymath.h"
#include "rlFrustum.h"
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
RLFrustum TerrainManager::frustum;

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

        float height = terrainNoise(scaledX + offsetx * 10, scaledZ + offsety * 10, 200.0f);
        vertexData[i + 1] = height * HEIGHT_FACTOR;
    }
    memcpy(mesh.vertices, vertexData, mesh.vertexCount * 3 * sizeof(float));
    UpdateMeshBuffer(mesh, 0, vertexData, mesh.vertexCount * 3 * sizeof(float), 0);
    terrain = LoadModelFromMesh(mesh);
    bbox = GetMeshBoundingBox(mesh);
    bbox.min = Vector3Add(bbox.min, position);
    bbox.max = Vector3Add(bbox.max, position);
    terrain.transform = MatrixTranslate(position.x, position.y, position.z);
    grass.InitGrass(offsetx, offsety, NOISE_SCALE, HEIGHT_FACTOR, WIDTH, HEIGHT);
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
    Shader ts = ResourceManager::getInstance().getShader("terrainShader");
    terrain.materials[0].shader = ts;
    fogDensityLoc = GetShaderLocation(ts, "fogDensity");
    // lightDirLoc = GetShaderLocation(ts, "lightDir");
    // lightColorLoc = GetShaderLocation(ts, "lightColor");
    ambientLoc = GetShaderLocation(ts, "ambient");
    viewPosLoc = GetShaderLocation(ts, "viewPos");
}

BoundingBox &Terrain::getBBox()
{
    return bbox;
}

void Terrain::UpdateShader(Camera3D &camera)
{
    Shader ts = ResourceManager::getInstance().getShader("terrainShader");
    SetShaderValue(ts, fogDensityLoc, &grass.fogDensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(ts, ambientLoc, &grass.ambient, SHADER_UNIFORM_VEC4);
    SetShaderValue(ts, viewPosLoc, &camera.position, SHADER_UNIFORM_VEC3);
}

void Terrain::DrawGrass(Camera3D &camera)
{
    grass.DrawGrass(camera);
}

Terrain::~Terrain()
{
    free(vertexData);
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
    // updateCords(currentTerrain);
}

void TerrainManager::DrawTerrains()
{
    Player player = Player::getInstance();
    frustum.Extract();
    // std::cout << "[TERRAINS BEING DRAWN]:\n";
    for (auto &[pos, terrain] : terrains)
    {
        if (frustum.AABBoxIn(terrain->getBBox().min, terrain->getBBox().max))
        {
            float distanceSq = Vector3DistanceSqr(player.GetPlayerPosition(), terrain->getPosition());
            if (distanceSq < VIEW_BOX_DISTANCE * VIEW_BOX_DISTANCE)
            {
                // std::cout << "(" << pos.x << "," << pos.y << ") ";
                terrain->UpdateShader(player.camera);
                DrawModel(terrain->getTerrain(), Vector3Zero(), 1.0f, DARKBROWN);
                // DrawBoundingBox(terrain->getBBox(), GREEN);
                // rlDisableBackfaceCulling();
                terrain->DrawGrass(player.camera);
                // rlEnableBackfaceCulling();
            }
        }
    }
    // std::cout << "\n[END]\n ";
}

void TerrainManager::DrawTerrainGrid()
{
    DrawGrid(10, 50);
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
