#pragma once
#include "raylib.h"

inline bool operator==(const Vector2 &a, const Vector2 &b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2 &a, const Vector2 &b)
{
    return a.x != b.x || a.y != b.y;
}

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
}