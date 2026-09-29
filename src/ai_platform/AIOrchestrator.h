#pragma once
#include "ai_platform/ForgeAI.h"
#include <filesystem>
#include <string>
#include <vector>

namespace forge::aiplatform {

enum class ProviderKind { Local, Cloud, Custom };
enum class Permission { ReadProject, ModifyProject, ExecuteTools, Build, Publish };

struct ProviderProfile {
    std::string id{"local-default"};
    ProviderKind kind{ProviderKind::Local};
    std::string displayName{"Forge Local Provider"};
    bool connected{true};
};

struct ProjectSnapshot {
    std::filesystem::path root;
    std::size_t files{0};
    std::size_t scenes{0};
    std::size_t assets{0};
    std::size_t scripts{0};
    bool projectFileValid{false};
};

struct AgentTask {
    std::string title;
    std::string detail;
    std::vector<Permission> permissions;
    bool destructive{false};
};

class AIOrchestrator {
public:
    void setProvider(const ProviderProfile& provider) { provider_ = provider; }
    const ProviderProfile& provider() const { return provider_; }
    ProjectSnapshot inspect(const std::filesystem::path& projectRoot) const;
    std::vector<AgentTask> plan(Mode mode, const std::string& prompt, const ProjectSnapshot& snapshot) const;
    bool permissionRequired(Permission permission) const;
    static std::string permissionName(Permission permission);
private:
    ProviderProfile provider_{};
};

} // namespace forge::aiplatform
