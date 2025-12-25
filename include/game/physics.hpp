#include "raylib.h"
#include "game/scene.hpp"

class PhysicsSystem {
  public:
    Vector3 gravity;
    void Update(Scene *scene);

};
