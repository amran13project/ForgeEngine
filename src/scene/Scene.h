#pragma once
#include "core/Math.h"
#include <filesystem>
#include <string>
#include <vector>
namespace forge::scene {
using Vec3=forge::math::Vec3;
struct Entity { int id{}; std::string name; std::string type{"Entity"}; Vec3 position{},rotation{},scale{1,1,1}; bool visible{true},locked{false},selected{false}; };
class Scene {public: bool load(const std::filesystem::path&,std::string&); bool save(const std::filesystem::path&,std::string&) const; Entity* selected(); const Entity* selected() const; Entity& add(const std::string&,const std::string&); Entity& addCube(); void removeSelected(); void select(int); void clear(); std::vector<Entity>& entities(){return entities_;} const std::vector<Entity>& entities()const{return entities_;} private: std::vector<Entity>entities_;int nextId_{1};};
}
