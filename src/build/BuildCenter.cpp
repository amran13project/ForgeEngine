#include "build/BuildCenter.h"

#include <fstream>
#include <system_error>

namespace forge::build {

namespace {

bool isGeneratedDirectory(const std::filesystem::path& relative) {
    if (relative.empty()) return false;
    const auto first = relative.begin()->string();
    return first == "Builds" || first == "Cache" || first == "Logs" ||
           first == "Saves" || first == "Publishing";
}

bool packageProject(const std::filesystem::path& root,
                    const std::filesystem::path& destination,
                    std::size_t& fileCount,
                    std::string& error) {
    std::error_code ec;
    fileCount = 0;
    for (std::filesystem::recursive_directory_iterator it(
             root, std::filesystem::directory_options::skip_permission_denied, ec),
         end;
         it != end && !ec;
         it.increment(ec)) {
        const auto source = it->path();
        const auto relative = std::filesystem::relative(source, root, ec);
        if (ec) {
            error = "Unable to resolve project path: " + ec.message();
            return false;
        }
        if (isGeneratedDirectory(relative)) {
            if (it->is_directory(ec)) it.disable_recursion_pending();
            continue;
        }
        if (!it->is_regular_file(ec)) continue;

        const auto target = destination / relative;
        std::filesystem::create_directories(target.parent_path(), ec);
        if (ec) {
            error = "Unable to create build path: " + ec.message();
            return false;
        }
        std::filesystem::copy_file(
            source, target, std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            error = "Failed to package " + relative.string() + ": " + ec.message();
            return false;
        }
        ++fileCount;
    }
    if (ec) {
        error = "Project enumeration failed: " + ec.message();
        return false;
    }
    return true;
}

} // namespace

BuildResult BuildCenter::build(const std::filesystem::path& projectRoot,
                               const std::filesystem::path& outputDirectory,
                               const std::string& configuration) const {
    BuildResult result;
    result.outputDirectory = outputDirectory;
    std::error_code ec;
    const auto root = std::filesystem::absolute(projectRoot, ec);
    if (ec || !std::filesystem::exists(root, ec) || !std::filesystem::is_directory(root, ec)) {
        result.message = "Project root does not exist or is not a directory";
        return result;
    }
    if (!std::filesystem::is_regular_file(root / "ForgeProject.json", ec)) {
        result.message = "ForgeProject.json is missing";
        return result;
    }
    std::filesystem::create_directories(outputDirectory, ec);
    if (ec) {
        result.message = "Unable to create build output directory: " + ec.message();
        return result;
    }
    const auto payload = outputDirectory / "Project";
    std::filesystem::remove_all(payload, ec);
    if (ec) {
        result.message = "Unable to clear previous build payload: " + ec.message();
        return result;
    }
    std::filesystem::create_directories(payload, ec);
    if (ec) {
        result.message = "Unable to create build payload directory: " + ec.message();
        return result;
    }

    if (!packageProject(root, payload, result.files, result.message)) return result;

    std::ofstream manifest(outputDirectory / "BuildManifest.txt", std::ios::trunc);
    if (!manifest) {
        result.message = "Unable to write BuildManifest.txt";
        return result;
    }
    manifest << "Forge Engine Build Manifest\n"
             << "Configuration: " << (configuration.empty() ? "Development" : configuration) << "\n"
             << "Project: " << root.filename().string() << "\n"
             << "Engine: Forge Engine Professional 3.1\n"
             << "Files packaged: " << result.files << "\n"
             << "Payload: Project/\n"
             << "Status: SUCCESS\n";
    result.success = true;
    result.message = "Project packaged successfully";
    return result;
}

} // namespace forge::build
