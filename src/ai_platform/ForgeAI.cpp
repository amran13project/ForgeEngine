#include "ai_platform/ForgeAI.h"
#include <fstream>
namespace forge::aiplatform {
std::string ForgeAI::modeName(Mode m) const { switch(m){case Mode::Ask:return"Ask";case Mode::Explain:return"Explain";case Mode::Generate:return"Generate";case Mode::Modify:return"Modify";case Mode::Debug:return"Debug";case Mode::Optimize:return"Optimize";case Mode::Design:return"Design";case Mode::Test:return"Test";case Mode::Build:return"Build";case Mode::Publish:return"Publish";case Mode::Review:return"Review";} return"Ask"; }
AIResponse ForgeAI::ask(Mode mode, const std::string& prompt) {
    AIResponse r; r.plan.summary = "Forge AI inspected the request and prepared a safe, project-aware action plan.";
    if (!projectRoot_.empty() && std::filesystem::exists(projectRoot_ / "ForgeProject.json")) r.plan.files.push_back(projectRoot_ / "ForgeProject.json");
    r.text = "Forge AI " + modeName(mode) + ": " + prompt + "\n\nProvider status: local orchestration foundation ready. Connect a local or cloud model provider for generative execution.\nProject-aware safety: changes can be previewed, snapshotted and rolled back.";
    r.needsConfirmation = (mode == Mode::Modify || mode == Mode::Build || mode == Mode::Publish || mode == Mode::Generate);
    return r;
}
} // namespace forge::aiplatform
