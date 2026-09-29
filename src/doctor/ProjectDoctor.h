#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace forge::doctor {enum class Severity{Info,Warning,Error};struct Issue{Severity severity;std::string code,message,location;};class ProjectDoctor{public:std::vector<Issue>scan(const std::filesystem::path&project) const;};}
