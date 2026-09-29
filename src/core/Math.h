#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace forge::math {
struct Vec3 {
    float x{0}, y{0}, z{0};
    Vec3 operator+(const Vec3& o) const { return {x+o.x,y+o.y,z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x,y-o.y,z-o.z}; }
    Vec3 operator*(float s) const { return {x*s,y*s,z*s}; }
    Vec3& operator+=(const Vec3& o){ x+=o.x; y+=o.y; z+=o.z; return *this; }
};
inline float length(const Vec3& v){ return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); }
inline Vec3 normalized(const Vec3& v){ float l=length(v); return l>0.00001f ? v*(1.0f/l) : Vec3{}; }
struct Color { uint8_t r{255},g{255},b{255},a{255}; };
}
