#include "game/weapon.hpp"
#include "game/resource.hpp"
#include "raymath.h"


void Weapon::InitKatana()
{
    model = &ResourceManager::getInstance().getModel("katana");
    scale = 2.0f;
    position = Vector3{10.0f, 5.0f, 10.0f};
    animIndex = 0;
    animFPS = 0.01f;
    animCurrentFrame = 0;
    animTime = 0.0f;
}

void Weapon::Draw(Transform &transform, Quaternion &rotation, Matrix &worldMatrix, SceneContext *sceneCtx)
{
    Quaternion inRotation = rotation;
    Quaternion outRotation = transform.rotation;
    Quaternion rotate = QuaternionMultiply(outRotation, QuaternionInvert(inRotation));
    Matrix matrixTransform = QuaternionToMatrix(rotate);

    matrixTransform = MatrixMultiply(
        matrixTransform, MatrixTranslate(transform.translation.x, transform.translation.y, transform.translation.z));
    matrixTransform = MatrixMultiply(matrixTransform, worldMatrix);

    for (int i = 0; i < model->materialCount; i++)
    {
        model->materials[i].shader = sceneCtx->terrainShader->getShader();
    }
    DrawMesh(model->meshes[0], model->materials[1], matrixTransform);
}

Vector3 Weapon::getPosition()
{
    return position;
}
