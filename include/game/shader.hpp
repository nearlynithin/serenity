#pragma once

#include "raylib.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace gfx
{

class Shader
{
  private:
    ::Shader shader = {};
    std::string name;
    std::unordered_map<std::string, int> uniforms;

  public:
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&) = default;
    Shader &operator=(Shader &&) = default;

    // default constructor
    Shader()
      : shader{}
    {
    }
    Shader(std::string shaderName, const char *vsName, const char *fsName)
    {
        shader = ::LoadShader(vsName, fsName);
        name = shaderName;
    }

    void setLocation(int locIndex, int value)
    {
        shader.locs[locIndex] = value;
    }
    ::Shader &getShader()
    {
        return shader;
    }
    int addUniform(std::string name)
    {
        int loc = ::GetShaderLocation(shader, name.c_str());
        uniforms[name] = loc;
        return loc;
    }
    int getUniformLoc(std::string name)
    {
        auto it = uniforms.find(name);
        if (it == uniforms.end())
        {
            std::cout << "uniform " << name << " not found in " << this->name << "\n";
        }
        return it->second;
    }

    void SetShaderValue(std::string uniformName, const void *value, int uniformType)
    {
        ::SetShaderValue(shader, getUniformLoc(uniformName), value, uniformType);
    }
};

} // namespace gfx
