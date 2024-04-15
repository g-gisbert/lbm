#include "vec2.h"

#include <iostream>
#include <cmath>

Vec2 operator+(const Vec2& v1, const Vec2& v2) {
    return Vec2{v1.x + v2.x, v1.y + v2.y};
}

Vec2 operator-(const Vec2& v1, const Vec2& v2) {
    return Vec2{v1.x - v2.x, v1.y - v2.y};
}

Vec2 operator*(const Vec2& v, float s) {
    return Vec2{v.x * s, v.y * s};
}

Vec2 operator*(float s, const Vec2& v) {
    return Vec2{v.x * s, v.y * s};
}

Vec2 operator/(const Vec2& v, float s) {
    if (std::abs(s) < 10e-8)
        s = 10e-8;
    return Vec2{v.x / s, v.y / s};
}

float dot(const Vec2& v1, const Vec2& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

float cross(const Vec2& v1, const Vec2& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

float norm(const Vec2& v) {
    return std::sqrt(norm2(v));
}

float norm2(const Vec2& v) {
    return v.x * v.x + v.y * v.y;
}

Vec2 normalize(const Vec2& v) {
    float n = norm(v);
    return v / n;
}

std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}