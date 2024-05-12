#ifndef EFS_COLOR_H
#define EFS_COLOR_H

#include <iostream>

struct Color {
    float r;
    float g;
    float b;

    static Color zero() {
        return Color{0.0f, 0.0f, 0.0f};
    }
};


Color operator+(const Color& c1, const Color& c2);
Color operator-(const Color& c1, const Color& c2);
Color operator*(const Color& c, float s);
Color operator*(float s, const Color& c);
Color operator/(const Color& c, float s);

float dot(const Color& c1, const Color& c2);


float norm(const Color& c);
float norm2(const Color& c);
Color normalize(const Color& c);

std::ostream& operator<<(std::ostream& os, const Color& c);

#endif //EFS_COLOR_H
