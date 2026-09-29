#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace forge::plugins {struct Plugin{std::string id,name,version;std::filesystem::path path;bool enabled{true};};class PluginManager{public:std::vector<Plugin>discover(const std::filesystem::path&root)const;};}
