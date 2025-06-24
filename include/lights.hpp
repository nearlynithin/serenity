#ifndef LIGHTS_H
#define LIGHTS_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

#define MAX_LIGHTS 5
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

struct ShaderUniforms
{
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int shininessLoc;
    int specularStrengthLoc;
    int metallicFactorLoc;
};

class Light
{
  private:
    // the [shaders + their uniforms] the light affects
    std::unordered_map<std::string, ShaderUniforms> uniformMap;
    static int lightCounter;
    int id;
    int type;
    int enabled;
    float shininess = 25.0;
    float specularStrength = 0.5;
    float metallicFactor = 0.5;
    Vector3 position;
    Vector3 target;
    Color color;
    // float attenuation;

  public:
    Light(int type, Vector3 position, Vector3 target, Color color, std::vector<std::string> &shaders);
    bool isEnabled();
    Vector3 getPosition();
    Color getColor();
    void toggle();
    void UpdateLightShader();
};

#endif