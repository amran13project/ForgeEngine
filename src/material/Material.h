#pragma once
#include <string>
namespace forge::material {
struct Material { std::string name{"DefaultMaterial"}; float r{1},g{1},b{1},metallic{0},roughness{0.5f},emission{0}; bool transparent{false}; };
}
