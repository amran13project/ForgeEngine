#include "core/ForgeSystem.h"
#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include "assets/AssetDatabase.h"
#include "physics/Physics.h"
#include "scripting/VisualScript.h"
#include "network/NetworkLab.h"
#include "profiler/Profiler.h"
#include "build/BuildCenter.h"
#include "doctor/ProjectDoctor.h"
#include "recovery/Recovery.h"
#include "testing/TestingCenter.h"
#include "world/WorldGenerator.h"
#include "publish/PublishCenter.h"
#include "compliance/ComplianceChecker.h"
#include "localization/LocalizationManager.h"
#include "security/SecurityCenter.h"
#include "account/AccountPolicy.h"
#include "ai_platform/AIOrchestrator.h"

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace forge;

static fs::path makeRoot() {
    const auto stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const auto root = fs::temp_directory_path() / ("ForgeProfessionalQA_" + std::to_string(stamp));
    fs::create_directories(root);
    return root;
}

int main() {
    int passed = 0;
    int checks = 0;
    const auto root = makeRoot();
    std::error_code ec;

    auto check = [&](bool condition, const char* label) {
        ++checks;
        if (!condition) {
            std::cerr << "FAIL: " << label << "\n";
            fs::remove_all(root, ec);
            return false;
        }
        ++passed;
        std::cout << "PASS: " << label << "\n";
        return true;
    };

    core::ForgeSystem system;
    if (!check(system.initialize(), "engine initialization")) return 1;
    if (!check(system.isReady("Core") && system.isReady("Renderer") && system.isReady("Asset Database"), "core systems ready")) return 1;

    project::ProjectManager pm;
    project::ProjectInfo project;
    std::string error;
    if (!check(pm.createProject("Professional QA Game", root, "3D", project, error), "project creation")) return 1;
    if (!check(fs::exists(project.root / "ForgeProject.json"), "project metadata exists")) return 1;
    if (!check(fs::exists(project.root / "Scenes/Main.forgeScene"), "default scene exists")) return 1;
    if (!check(pm.openProject(project.root, project, error), "project reopen")) return 1;

    scene::Scene scene;
    if (!check(scene.load(project.root / project.defaultScene, error), "scene load")) return 1;
    if (!check(scene.entities().size() >= 3, "default scene entity count")) return 1;
    auto& cube = scene.addCube();
    cube.position = {2.0f, 3.0f, -1.0f};
    cube.rotation = {0.0f, 45.0f, 0.0f};
    cube.scale = {1.5f, 2.0f, 0.5f};
    scene.select(cube.id);
    if (!check(scene.selected() && scene.selected()->id == cube.id, "entity selection")) return 1;
    if (!check(scene.save(project.root / project.defaultScene, error), "scene save")) return 1;

    scene::Scene reopenedScene;
    if (!check(reopenedScene.load(project.root / project.defaultScene, error), "scene persistence reload")) return 1;
    const auto* reopenedCube = reopenedScene.selected();
    if (!check(reopenedCube && reopenedCube->name == "Cube", "selected cube persisted")) return 1;
    if (!check(reopenedCube && reopenedCube->position.x == 2.0f && reopenedCube->position.y == 3.0f && reopenedCube->position.z == -1.0f, "transform persistence")) return 1;
    if (!check(reopenedCube && reopenedCube->rotation.y == 45.0f && reopenedCube->scale.x == 1.5f, "rotation/scale persistence")) return 1;

    assets::AssetDatabase db;
    if (!check(db.scan(project.root / "Assets", error), "asset database scan")) return 1;

    physics::PhysicsWorld physics;
    physics.addBody({cube.id, {0, 2, 0}, {0, 0, 0}, {.5f, .5f, .5f}, 1.0f, true});
    physics.step(0.1f);
    if (!check(!physics.bodies().empty() && physics.bodies()[0].position.y < 2.0f, "physics integration")) return 1;

    scripting::Graph graph;
    graph.add({scripting::Op::Set, "score", 2.0f, ""});
    graph.add({scripting::Op::Add, "score", 3.0f, ""});
    std::unordered_map<std::string, float> vars;
    std::vector<std::string> graphLog;
    if (!check(graph.execute(vars, graphLog), "visual script execution")) return 1;
    if (!check(vars["score"] == 5.0f, "visual script state mutation")) return 1;

    network::NetworkLab network;
    network.configure(30, 0, 0);
    network.send("qa", 1);
    network.tick(2);
    if (!check(network.stats().sent == 1 && network.stats().delivered == 1, "network delivery")) return 1;

    profiler::Profiler profiler;
    profiler.beginFrame();
    profiler.sample("ProfessionalQA", 0.25);
    profiler.endFrame();
    if (!check(profiler.frameMs() >= 0 && profiler.fps() >= 0, "profiler sample")) return 1;

    const auto buildOut = project.root / "Builds" / "QA";
    auto build = build::BuildCenter().build(project.root, buildOut, "QA");
    if (!check(build.success, "build center package")) return 1;
    if (!check(fs::exists(buildOut / "BuildManifest.txt"), "build manifest exists")) return 1;
    if (!check(fs::exists(buildOut / "Project/ForgeProject.json"), "build payload contains project")) return 1;

    auto doctorIssues = doctor::ProjectDoctor().scan(project.root);
    if (!check(!doctorIssues.empty(), "project doctor returns a diagnostic result")) return 1;

    auto snapshot = recovery::Recovery().snapshot(project.root, "professional-qa", error);
    if (!check(!snapshot.empty() && fs::exists(snapshot), "recovery snapshot")) return 1;

    world::WorldGenerator().generate(scene, "forest village");
    if (!check(scene.entities().size() > 3, "procedural world generation")) return 1;

    testing::TestingCenter testCenter;
    testCenter.add("QA truth", [] { return true; });
    auto results = testCenter.run();
    if (!check(results.size() == 1 && results[0].passed, "testing center")) return 1;

    publish::StoreProfile direct;
    direct.store = publish::Store::Direct;
    direct.appName = "Professional QA Game";
    direct.publisher = "Forge Creator";
    direct.description = "QA test artifact";
    direct.version = "1.0.0";
    direct.buildNumber = "1";
    auto plan = publish::PublishCenter().validate(direct, project.root);
    bool blocking = false;
    for (const auto& issue : plan.issues) if (issue.error) blocking = true;
    if (!check(!blocking, "publish validation")) return 1;
    if (!check(publish::PublishCenter().writeSubmissionManifest(direct, project.root, error), "publish submission manifest")) return 1;

    auto compliance = compliance::ComplianceChecker().scan(direct, project.root);
    if (!check(compliance.ready(), "compliance baseline")) return 1;

    if (!check(localization::LocalizationManager::supportedLanguages().size() >= 5, "localization catalog")) return 1;

    auto security = security::SecurityCenter().scan(project.root);
    if (!check(!security.secretsDetected, "security secret scan baseline")) return 1;

    if (!check(account::AccountPolicy::isValidDateOfBirth("01/01/2000"), "DOB validation")) return 1;
    if (!check(!account::AccountPolicy::isValidDateOfBirth("31/02/2000"), "invalid DOB rejection")) return 1;
    if (!check(account::AccountPolicy::birthdayMatches("01/01/2000", "01/01/2026"), "birthday matching")) return 1;

    aiplatform::AIOrchestrator orchestrator;
    const auto projectSnapshot = orchestrator.inspect(project.root);
    if (!check(projectSnapshot.projectFileValid && projectSnapshot.files > 0, "AI project inspection")) return 1;
    const auto aiPlan = orchestrator.plan(aiplatform::Mode::Publish, "prepare QA release", projectSnapshot);
    if (!check(aiPlan.size() >= 2, "AI publish plan")) return 1;
    if (!check(orchestrator.permissionRequired(aiplatform::Permission::Publish), "AI publish permission boundary")) return 1;

    system.shutdown();
    fs::remove_all(root, ec);
    std::cout << "\nForgeProfessionalQA: " << passed << "/" << checks << " checks passed.\n";
    return passed == checks ? 0 : 2;
}
