#pragma once
#include "raylib.h"

namespace std
{
template <> struct hash<Vector2>
{
    std::size_t operator()(const Vector2 &v) const
    {
        std::size_t hx = std::hash<int>()(static_cast<int>(v.x * 1000));
        std::size_t hy = std::hash<int>()(static_cast<int>(v.y * 1000));
        return hx ^ (hy << 1);
    }
};
} // namespace std
