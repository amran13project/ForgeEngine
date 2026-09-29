#pragma once
#include <filesystem>
#include <unordered_map>
namespace forge::hotreload {
class Watcher {
public:
    bool changed(const std::filesystem::path& path);
    void forget(const std::filesystem::path& path);
private:
    std::unordered_map<std::filesystem::path,std::filesystem::file_time_type> stamps_;
};
}
