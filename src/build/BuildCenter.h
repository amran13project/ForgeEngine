#pragma once
#include <cstddef>
#include <filesystem>
#include <string>

namespace forge::build {

struct BuildResult {
    bool success{false};
    std::size_t files{0};
    std::string message;
    std::filesystem::path outputDirectory;
};

class BuildCenter {
public:
    BuildResult build(const std::filesystem::path& projectRoot,
                      const std::filesystem::path& outputDirectory,
                      const std::string& configuration) const;
};

} // namespace forge::build
