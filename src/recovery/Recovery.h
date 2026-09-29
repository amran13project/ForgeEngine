#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace forge::recovery {class Recovery{public:std::filesystem::path snapshot(const std::filesystem::path&source,const std::string&label,std::string&error) const;std::vector<std::filesystem::path> list(const std::filesystem::path&project)const;};}
