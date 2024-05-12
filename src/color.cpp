#include "color.h"


#include <iostream>
#include <cmath>

Color operator+(const Color& c1, const Color& c2) {
    return Color{c1.r + c2.r, c1.g + c2.g, c1.b + c2.b};
}

Color operator-(const Color& c1, const Color& c2) {
    return Color{c1.r - c2.r, c1.g - c2.g, c1.b - c2.b};
}

Color operator*(const Color& c, float s) {
    return Color{c.r * s, c.g * s, c.b * s};
}

Color operator*(float s, const Color& c) {
    return Color{c.r * s, c.g * s, c.b * s};
}

Color operator/(const Color& c, float s) {
    if (std::abs(s) < 10e-8)
        s = 10e-8;
    return Color{c.r / s, c.g / s, c.b / s};
}

float dot(const Color& c1, const Color& c2) {
    return c1.r * c2.r + c1.g * c2.g + c1.b * c2.b;
}

float norm(const Color& v) {
    return std::sqrt(norm2(v));
}

float norm2(const Color& c) {
    return c.r * c.r + c.g * c.g + c.b * c.b;
}

Color normalize(const Color& c) {
    float n = norm(c);
    return c / n;
}

std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << "(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}