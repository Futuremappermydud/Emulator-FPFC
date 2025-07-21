#pragma once

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Quaternion.hpp"

inline void operator+=(UnityEngine::Vector3& lhs, const UnityEngine::Vector3& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
}

inline UnityEngine::Vector3 operator+(const UnityEngine::Vector3& lhs, const UnityEngine::Vector3& rhs)
{
    return UnityEngine::Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline UnityEngine::Vector3 operator*(const UnityEngine::Quaternion& lhs, const UnityEngine::Vector3& rhs)
{
    return UnityEngine::Quaternion::op_Multiply(lhs, rhs);
}

inline UnityEngine::Vector3 operator*(const UnityEngine::Vector3& lhs, const float rhs)
{
    return UnityEngine::Vector3(lhs.x * rhs, lhs .y * rhs, lhs.z * rhs);
}

inline UnityEngine::Vector2 operator*(const UnityEngine::Vector2& lhs, const float rhs)
{
    return UnityEngine::Vector2(lhs.x * rhs, lhs.y * rhs);
}