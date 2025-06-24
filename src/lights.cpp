#include "lights.hpp"
#include "game/resource.hpp"
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>

int Light::lightCounter = 0;

Light::Light(int type, Vector3 position, Vector3 target, Color color, std::vector<std::string> &shaders)
{
    if (lightCounter >= MAX_LIGHTS)
    {
        TraceLog(LOG_ERROR, "Max number of lights exceeded");
    }

    auto rm = ResourceManager::getInstance();
    id = lightCounter++;

    enabled = true;
    this->type = type;
    this->position = position;
    this->target = target;
    this->color = color;
    // attenuation = attenuation;

    for (const auto &shader_name : shaders)
    {
        Shader shader = rm.getShader(shader_name);
        ShaderUniforms shaderUniform = {
            .enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", id)),
            .typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", id)),
            .positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", id)),
            .targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", id)),
            .colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", id)),
            .shininessLoc = GetShaderLocation(shader, TextFormat("shininess", id)),
            .specularStrengthLoc = GetShaderLocation(shader, TextFormat("specularStrength", id)),
            .metallicFactorLoc = GetShaderLocation(shader, TextFormat("metallicFactor", id)),
        };

        SetShaderValue(shader, shaderUniform.shininessLoc, &shininess, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, shaderUniform.specularStrengthLoc, &specularStrength, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, shaderUniform.metallicFactorLoc, &metallicFactor, SHADER_UNIFORM_FLOAT);

        // attenuationLoc = GetShaderLocation(*shader, TextFormat("lights[%i].attenuation", id));
        if (!uniformMap.count(shader_name))
        {
            uniformMap[shader_name] = shaderUniform;
        }
    }
    UpdateLightShader();
}

void Light::UpdateLightShader()
{
    auto rm = ResourceManager::getInstance();
    for (const auto &[name, uniform] : uniformMap)
    {
        Shader shader = rm.getShader(name);
        // Send to shader light enabled state and type
        SetShaderValue(shader, uniform.enabledLoc, &enabled, SHADER_UNIFORM_INT);
        SetShaderValue(shader, uniform.typeLoc, &type, SHADER_UNIFORM_INT);

        // Send to shader light position values
        float lightPos[3] = {position.x, position.y, position.z};
        SetShaderValue(shader, uniform.positionLoc, &lightPos, SHADER_UNIFORM_VEC3);

        // Send to shader light target position values
        float lightTarget[3] = {target.x, target.y, target.z};
        SetShaderValue(shader, uniform.targetLoc, lightTarget, SHADER_UNIFORM_VEC3);

        // Send to shader light color values
        float lightColor[4] = {(float)color.r / (float)255, (float)color.g / (float)255, (float)color.b / (float)255,
                               (float)color.a / (float)255};
        SetShaderValue(shader, uniform.colorLoc, lightColor, SHADER_UNIFORM_VEC4);

        // Send to shader Light attenuation value
        // SetShaderValue(*shader, attenuationLoc, &attenuation, SHADER_UNIFORM_FLOAT);
    }
}

bool Light::isEnabled()
{
    return (enabled == 1) ? true : false;
}

Vector3 Light::getPosition()
{
    return position;
}

Color Light::getColor()
{
    return color;
}

void Light::toggle()
{
    enabled = !enabled;
}