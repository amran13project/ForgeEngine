#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace forge::aiplatform {

enum class Mode { Ask, Explain, Generate, Modify, Debug, Optimize, Design, Test, Build, Publish, Review };
struct ChangePlan { std::string summary; std::vector<std::filesystem::path> files; bool destructive{false}; };
struct AIResponse { std::string text; ChangePlan plan; bool needsConfirmation{false}; };

class ForgeAI {
public:
    void setProject(const std::filesystem::path& root) { projectRoot_ = root; }
    AIResponse ask(Mode mode, const std::string& prompt);
    std::string modeName(Mode mode) const;
private:
    std::filesystem::path projectRoot_{};
};

} // namespace forge::aiplatform
