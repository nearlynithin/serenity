#include "game/grass.hpp"
#include "game/resource.hpp"
#include "perlin.hpp"
#include "raylib.h"
void Grass::InitGrass(float offset_x, float offset_y, float noise, float heightFactor, float width, float height,
                      Mesh &mesh)
{
    grass = LoadModelFromMesh(GenMeshCube(0.2, 0.2, 0.2));
    transforms = (Matrix *)RL_CALLOC(MAX_INSTANCES, sizeof(Matrix));

    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        Vector3 grassPos;
        float jitter = 0.15f;

        if (i < mesh.vertexCount)
        {
            int vertexIndex = i * 3;
            grassPos.x = mesh.vertices[vertexIndex] + (width / 2 + offset_x);
            grassPos.y = mesh.vertices[vertexIndex + 1];
            grassPos.z = mesh.vertices[vertexIndex + 2] + (height / 2 + offset_y);
        }
        else
        {
            int baseIndex = (i % (mesh.vertexCount * 2)) * 3;

            if (baseIndex < mesh.vertexCount * 3)
            {
                Vector3 p1, p2, p3;

                p1.x = mesh.vertices[baseIndex] + (width / 2 + offset_x);
                p1.y = mesh.vertices[baseIndex + 1];
                p1.z = mesh.vertices[baseIndex + 2] + (height / 2 + offset_y);

                int idx2 = baseIndex + 3;
                p2.x = mesh.vertices[idx2] + (width / 2 + offset_x);
                p2.y = mesh.vertices[idx2 + 1];
                p2.z = mesh.vertices[idx2 + 2] + (height / 2 + offset_y);

                int idx3 = baseIndex + 2 * 3;
                p3.x = mesh.vertices[idx3] + (width / 2 + offset_x);
                p3.y = mesh.vertices[idx3 + 1];
                p3.z = mesh.vertices[idx3 + 2] + (height / 2 + offset_y);

                float u = (float)GetRandomValue(0, 100) / 100.0f;
                float v = (float)GetRandomValue(0, (int)((1.0f - u) * 100)) / 100.0f;
                float w = 1.0f - u - v;

                grassPos.x = u * p1.x + v * p2.x + w * p3.x;
                grassPos.y = u * p1.y + v * p2.y + w * p3.y;
                grassPos.z = u * p1.z + v * p2.z + w * p3.z;

                grassPos.x += (float)GetRandomValue(-(int)(jitter * 100), (int)(jitter * 100)) / 100.0f;
                grassPos.z += (float)GetRandomValue(-(int)(jitter * 100), (int)(jitter * 100)) / 100.0f;
            }
            else
            {
                int randomIndex = GetRandomValue(0, mesh.vertexCount - 1) * 3;
                grassPos.x = mesh.vertices[randomIndex] + (width / 2 + offset_x);
                grassPos.y = mesh.vertices[randomIndex + 1];
                grassPos.z = mesh.vertices[randomIndex + 2] + (height / 2 + offset_y);
                grassPos.x += (float)GetRandomValue(-(int)(jitter * 100), (int)(jitter * 100)) / 100.0f;
                grassPos.z += (float)GetRandomValue(-(int)(jitter * 100), (int)(jitter * 100)) / 100.0f;
            }
        }

        Matrix translation = MatrixTranslate(grassPos.x, grassPos.y, grassPos.z);
        Vector3 axis = {0.0f, 1.0f, 0.0f};
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        Matrix rotation = MatrixRotate(axis, angle);
        transforms[i] = MatrixMultiply(rotation, translation);
    }

    Shader shader = ResourceManager::getInstance().getShader("grassShader");
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    matInstances = grass.materials[0];
    matInstances.shader = shader;
}

void Grass::DrawGrass()
{
    DrawMeshInstanced(grass.meshes[0], matInstances, transforms, MAX_INSTANCES);
}

void Grass::UnloadGrass()
{
    RL_FREE(transforms);
    UnloadMaterial(matInstances);
    UnloadModel(grass);
}