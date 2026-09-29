#include "ai_platform/AIOrchestrator.h"
#include <algorithm>
#include <fstream>

namespace forge::aiplatform {

ProjectSnapshot AIOrchestrator::inspect(const std::filesystem::path& root) const {
    ProjectSnapshot s; s.root = root;
    if (root.empty() || !std::filesystem::exists(root)) return s;
    std::error_code ec;
    s.projectFileValid = std::filesystem::is_regular_file(root / "ForgeProject.json", ec);
    for (std::filesystem::recursive_directory_iterator it(root, ec), end; it != end && !ec; it.increment(ec)) {
        const auto& p = it->path();
        if (!std::filesystem::is_regular_file(p, ec)) continue;
        ++s.files;
        const auto ext = p.extension().string();
        if (ext == ".forgeScene") ++s.scenes;
        else if (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".cs" || ext == ".js") ++s.scripts;
        else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".fbx" || ext == ".obj" || ext == ".wav" || ext == ".ogg") ++s.assets;
    }
    return s;
}

std::vector<AgentTask> AIOrchestrator::plan(Mode mode, const std::string& prompt, const ProjectSnapshot& snapshot) const {
    std::vector<AgentTask> tasks;
    const std::string context = "Project contains " + std::to_string(snapshot.files) + " files, " +
        std::to_string(snapshot.scenes) + " scenes, " + std::to_string(snapshot.assets) + " assets and " +
        std::to_string(snapshot.scripts) + " script files.";
    switch (mode) {
        case Mode::Review:
        case Mode::Explain:
            tasks.push_back({"Inspect project", context, {Permission::ReadProject}, false});
            tasks.push_back({"Explain findings", prompt.empty() ? "Review project health and architecture." : prompt, {Permission::ReadProject}, false});
            break;
        case Mode::Debug:
            tasks.push_back({"Inspect diagnostics", context, {Permission::ReadProject}, false});
            tasks.push_back({"Trace probable cause", prompt, {Permission::ReadProject}, false});
            tasks.push_back({"Propose a reversible fix", "Generate a change plan without applying it.", {Permission::ReadProject, Permission::ModifyProject}, false});
            break;
        case Mode::Generate:
        case Mode::Modify:
        case Mode::Optimize:
        case Mode::Design:
            tasks.push_back({"Inspect relevant project context", context, {Permission::ReadProject}, false});
            tasks.push_back({"Prepare changes", prompt, {Permission::ReadProject, Permission::ModifyProject}, true});
            tasks.push_back({"Validate changes", "Run applicable tests before accepting the change.", {Permission::ReadProject, Permission::ExecuteTools}, false});
            break;
        case Mode::Test:
            tasks.push_back({"Run project tests", context, {Permission::ReadProject, Permission::ExecuteTools}, false});
            break;
        case Mode::Build:
            tasks.push_back({"Validate project", context, {Permission::ReadProject, Permission::ExecuteTools}, false});
            tasks.push_back({"Build release artifact", prompt, {Permission::Build}, true});
            break;
        case Mode::Publish:
            tasks.push_back({"Validate store requirements", context, {Permission::ReadProject}, false});
            tasks.push_back({"Prepare release package", prompt, {Permission::Build}, true});
            tasks.push_back({"Submit to selected store", "Requires explicit user approval and connected store credentials.", {Permission::Publish}, true});
            break;
        default:
            tasks.push_back({"Answer request", prompt, {Permission::ReadProject}, false});
            break;
    }
    return tasks;
}

bool AIOrchestrator::permissionRequired(Permission permission) const {
    if (permission == Permission::ReadProject) return false;
    if (permission == Permission::ExecuteTools || permission == Permission::Build || permission == Permission::Publish) return true;
    return true;
}

std::string AIOrchestrator::permissionName(Permission p) {
    switch (p) {
        case Permission::ReadProject: return "Read Project";
        case Permission::ModifyProject: return "Modify Project";
        case Permission::ExecuteTools: return "Execute Tools";
        case Permission::Build: return "Build";
        case Permission::Publish: return "Publish";
        default: return "Unknown";
    }
}

} // namespace forge::aiplatform
