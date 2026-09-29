#pragma once
#include "core/Math.h"
#include <array>
#include <vector>
namespace forge::renderer {
struct Camera { forge::math::Vec3 position{0,2,8}; float yaw{0},pitch{0}; float fov{70}; };
struct ProjectedPoint { int x{},y{}; float depth{}; };
class SoftwareRenderer {
public:
    void setCamera(const Camera& c){camera_=c;}
    ProjectedPoint project(const forge::math::Vec3& p,int cx,int cy) const;
    std::array<ProjectedPoint,8> cube(const forge::math::Vec3& pos,const forge::math::Vec3& scale,int cx,int cy) const;
private: Camera camera_{};
};
}
