#pragma once
#include "assets/AssetDatabase.h"
#include "build/BuildCenter.h"
#include "core/ForgeSystem.h"
#include "doctor/ProjectDoctor.h"
#include "network/NetworkLab.h"
#include "physics/Physics.h"
#include "platform/NativeWindow.h"
#include "profiler/Profiler.h"
#include "project/ProjectManager.h"
#include "recovery/Recovery.h"
#include "scene/Scene.h"
#include "testing/TestingCenter.h"
#include "ai/AI.h"
#include "world/WorldGenerator.h"
#include "plugins/PluginManager.h"
#include "renderer/SoftwareRenderer.h"
#include "account/AccountService.h"
#include "account/AccountPolicy.h"
#include "ai_platform/ForgeAI.h"
#include "ai_platform/AIOrchestrator.h"
#include "publish/PublishCenter.h"
#include "compliance/ComplianceChecker.h"
#include "localization/LocalizationManager.h"
#include "accessibility/AccessibilitySettings.h"
#include "security/SecurityCenter.h"
#include "release/ReleaseManager.h"
#include "workspace/WorkspaceProfile.h"
#include <string>
#include <vector>
#include <chrono>

enum class ForgeScreen { Login, Signup, Hub, CreateProject, Editor, Publish };
namespace forge::editor {
class ForgeEditor {
public:
    ForgeEditor(platform::NativeWindow&, core::ForgeSystem&);
    void run();
private:
    enum class Panel { Scene, Assets, Doctor, Profiler, AI, Network, Build, Tests, Systems, Publish, Security, Localization };
    void onEvent(const platform::InputEvent&);
    void paint(int,int); void paintAuth(bool signup); void paintHub(); void paintCreate(); void paintEditor(); void paintPublish();
    void drawPanel(int,int,int,int,uint32_t,uint32_t); void drawButton(int,int,int,int,const std::string&,bool=false);
    void createProject(); void openProject(); void saveScene(); void runBuild(); void runDoctor(); void runAI(); void runTests(); void runPublish(); void takeSnapshot(); void generateWorld(); void submitLogin(); void submitSignup();
    void beginPlay(); void stopPlay(); void tickPlay();
    void handleHubClick(int,int); void handleCreateClick(int,int); void handleEditorClick(int,int); void handlePublishClick(int,int); void handleAuthClick(int,int,bool); void handleChar(char32_t); void handleKey(platform::Key);
    void setStatus(const std::string&);
    platform::NativeWindow& window_; core::ForgeSystem& system_;
    ForgeScreen screen_{ForgeScreen::Login}; Panel panel_{Panel::Scene};
    project::ProjectInfo project_{}; scene::Scene scene_{}; assets::AssetDatabase assets_{}; physics::PhysicsWorld physics_{};
    profiler::Profiler profiler_{}; network::NetworkLab network_{}; renderer::SoftwareRenderer renderer_{}; doctor::ProjectDoctor doctor_{}; recovery::Recovery recovery_{}; plugins::PluginManager* pluginsPtr_{nullptr}; testing::TestingCenter tests_{}; ai::RuleAIProvider aiProvider_{}; world::WorldGenerator worldGen_{};
    account::AccountService account_{}; aiplatform::ForgeAI forgeAI_{}; aiplatform::AIOrchestrator aiOrchestrator_{}; publish::PublishCenter publisher_{}; compliance::ComplianceChecker compliance_{}; localization::LocalizationManager localization_{}; accessibility::AccessibilitySettings accessibility_{}; security::SecurityCenter security_{}; release::ReleaseManager releases_{}; workspace::WorkspaceProfile workspace_{}; publish::StoreProfile publishProfile_{};
    std::string status_{"Ready"}, log_{"[STARTUP] INFO - Forge Engine Professional platform online"}; std::string projectName_{"MyGame"}, projectLocation_{}; std::string authDisplay_, authEmail_, authPassword_, authDob_, authCountry_{"Malaysia"}; int activeField_{0}; int authField_{0}; bool playing_{false}; bool paused_{false}; double playTime_{0}; int mouseX_{-1}; int mouseY_{-1};
    std::vector<forge::math::Vec3> playStartPositions_{}; std::chrono::steady_clock::time_point lastPlayTick_{};
};
}
