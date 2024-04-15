#ifndef EFS_VEC2_H
#define EFS_VEC2_H

#include <iostream>

struct Vec2 {
    float x;
    float y;

    static Vec2 zero() {
        return Vec2{0.0, 0.0};
    }
};


Vec2 operator+(const Vec2& v1, const Vec2& v2);
Vec2 operator-(const Vec2& v1, const Vec2& v2);
Vec2 operator*(const Vec2& vec, float s);
Vec2 operator*(float s, const Vec2& vec);
Vec2 operator/(const Vec2& vec, float s);

float dot(const Vec2& v1, const Vec2& v2);
float cross(const Vec2& v1, const Vec2& v2);

float norm(const Vec2& v);
float norm2(const Vec2& v);
Vec2 normalize(const Vec2& v);

std::ostream& operator<<(std::ostream& os, const Vec2& v);

#endif //EFS_VEC2_H
