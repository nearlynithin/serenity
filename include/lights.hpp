#ifndef LIGHTS_H
#define LIGHTS_H

#include "raylib.h"
#include <string>

#define MAX_LIGHTS 5
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

class Light
{
  private:
    static int lightCounter;
    int id;
    int type;
    int enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    Shader *shader;
    // float attenuation;

    int typeLoc;
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    // int attenuationLoc;

  public:
    Light(int type, Vector3 position, Vector3 target, Color color, Shader &lightShader);
    Light(const Light&) = default;
    Light(Light&&) = default;
    Light& operator=(const Light&) = default;
    Light& operator=(Light&&) = default;


    bool isEnabled();
    Vector3 getPosition();
    Color getColor();
    void toggle();
    void UpdateLightShader();
};

#endif