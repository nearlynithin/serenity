#include "game/lights.hpp"
#include <iostream>

static std::unordered_map<std::string, Light> lights;
static int lightCounter = 0;

// Creates a light and returns the id of the light
int CreateLight(const std::string &lightName, int type, Vector3 position, Vector3 target, Color color,
                float attenuation, Shader shader)
{
    if (lightCounter >= MAX_LIGHTS)
    {
        std::cerr << "Max number of lights reached! Cannot create more lights.\n";
        return -1;
    }

    Light light;
    light.id = lightCounter++;
    light.enabled = 1;
    light.type = type;
    light.position = position;
    light.target = target;
    light.color = color;
    light.attenuation = attenuation;

    // NOTE: Shader parameters names for lights must match the requested ones
    light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", light.id));
    light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", light.id));
    light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", light.id));
    light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", light.id));
    light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", light.id));
    light.attenuationLoc = GetShaderLocation(shader, TextFormat("lights[%i].attenuation", light.id));

    lights[lightName] = light;

    UpdateLightValues(shader, light);

    return light.id;
}

void UpdateLightValues(Shader shader, Light light)
{
    // Send to shader light enabled state and type
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float position[3] = {light.position.x, light.position.y, light.position.z};
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float target[3] = {light.target.x, light.target.y, light.target.z};
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float color[4] = {(float)light.color.r / (float)255, (float)light.color.g / (float)255,
                      (float)light.color.b / (float)255, (float)light.color.a / (float)255};
    SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);

    // Send to shader Light attenuation value
    SetShaderValue(shader, light.attenuationLoc, &light.attenuation, SHADER_UNIFORM_FLOAT);
}

void UpdateLight(Shader shader, const std::string &lightName)
{
    auto it = lights.find(lightName);
    if (it == lights.end())
    {
        std::cerr << "Warning: Tried to update a non-existent light: " << lightName << std::endl;
        return;
    }

    Light &light = it->second;

    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

    float position[3] = {light.position.x, light.position.y, light.position.z};
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

    float target[3] = {light.target.x, light.target.y, light.target.z};
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
    float color[4] = {(float)light.color.r / (float)255, (float)light.color.g / (float)255,
                      (float)light.color.b / (float)255, (float)light.color.a / (float)255};
    SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, light.attenuationLoc, &light.attenuation, SHADER_UNIFORM_FLOAT);
}