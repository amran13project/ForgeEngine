#pragma once
#include "core/Math.h"
#include <vector>
namespace forge::physics {
struct Body { int entityId{}; forge::math::Vec3 position{},velocity{},halfExtents{0.5f,0.5f,0.5f}; float mass{1}; bool dynamic{true}; bool grounded{false}; };
struct Collision { int a{},b{}; };
class PhysicsWorld {
public: void clear(); void addBody(const Body&); void step(float dt); const std::vector<Body>& bodies()const{return bodies_;} std::vector<Collision> collisions() const;
private: std::vector<Body>bodies_;
};
}
