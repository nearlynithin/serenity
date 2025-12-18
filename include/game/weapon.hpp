#ifndef WEAPON_H
#define WEAPON_H

#include "game/resource.hpp"
#include "game/renderContext.hpp"
#include "raylib.h"

class Weapon
{
  private:
    Model *model;
    Matrix modelMatrix;
    Vector3 position;
    float scale;

    ModelAnim anims;
    float animFPS;
    int animCurrentFrame;
    int animIndex;
    float animTime;
    float xRotation;
    float yRotation;
    float zRotation;

  public:
    void InitKatana();
    void Draw(Transform &transform, Quaternion &rotation, Matrix &worldMatrix, SceneContext *sceneCtx);
    Vector3 getPosition();
};

#endif
