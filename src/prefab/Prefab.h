#pragma once
#include "scene/Scene.h"
#include <filesystem>
#include <string>
namespace forge::prefab {
class PrefabStore {
public:
    bool save(const scene::Entity& entity,const std::filesystem::path& file,std::string& error) const;
    bool load(scene::Entity& entity,const std::filesystem::path& file,std::string& error) const;
};
}
