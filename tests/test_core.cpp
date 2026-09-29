#include "core/ForgeSystem.h"
#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include "assets/AssetDatabase.h"
#include "physics/Physics.h"
#include "ai/AI.h"
#include "scripting/VisualScript.h"
#include "network/NetworkLab.h"
#include "profiler/Profiler.h"
#include "build/BuildCenter.h"
#include "doctor/ProjectDoctor.h"
#include "recovery/Recovery.h"
#include "plugins/PluginManager.h"
#include "testing/TestingCenter.h"
#include "world/WorldGenerator.h"
#include "publish/PublishCenter.h"
#include "compliance/ComplianceChecker.h"
#include "localization/LocalizationManager.h"
#include "security/SecurityCenter.h"
#include "release/ReleaseManager.h"
#include "workspace/WorkspaceProfile.h"
#include "accessibility/AccessibilitySettings.h"
#include "account/AccountService.h"
#include "account/AccountPolicy.h"
#include "ai_platform/AIOrchestrator.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <unordered_map>
using namespace forge;
int main(){
    core::ForgeSystem fsys; assert(fsys.initialize()); assert(fsys.isReady("Core")); assert(fsys.isReady("Renderer"));
    auto root=std::filesystem::temp_directory_path()/"ForgeEngineFull_Test"; std::error_code ec; std::filesystem::remove_all(root,ec); std::filesystem::create_directories(root);
    project::ProjectInfo p; std::string err; project::ProjectManager pm; if(!pm.createProject("Test Game",root,"3D",p,err)){std::cerr<<"createProject error: "<<err<<"\n";return 1;} assert(std::filesystem::exists(p.root/"ForgeProject.json"));
    project::ProjectInfo reopened; assert(pm.openProject(p.root,reopened,err)); assert(reopened.name=="Test_Game");
    scene::Scene scene; assert(scene.load(p.root/p.defaultScene,err)); assert(!scene.entities().empty()); auto&cube=scene.addCube(); cube.position.y=2; assert(scene.save(p.root/p.defaultScene,err));
    assets::AssetDatabase db; assert(db.scan(p.root/"Assets",err));
    physics::PhysicsWorld pw; pw.addBody({cube.id,{0,2,0},{0,0,0},{.5f,.5f,.5f},1,true}); pw.step(0.1f); assert(pw.bodies()[0].position.y<2);
    ai::Agent agent; agent.perception.seesPlayer=true; ai::BehaviorBrain brain; assert(brain.tick(agent,0.016f)==ai::NodeState::Running); assert(agent.state=="Chase");
    scripting::Graph g; g.add({scripting::Op::Set,"score",2.0f,""}); g.add({scripting::Op::Add,"score",3.0f,""}); std::unordered_map<std::string,float> vars; std::vector<std::string> log; assert(g.execute(vars,log)); assert(vars["score"]==5);
    network::NetworkLab net; net.configure(10,0,0); net.send("hello",0); net.tick(1); assert(net.stats().sent==1&&net.stats().delivered==1);
    profiler::Profiler prof; prof.beginFrame(); prof.sample("Test",0.2); prof.endFrame(); assert(prof.frameMs()>=0);
    auto report=build::BuildCenter().build(p.root,p.root/"Builds"/"Test","Debug"); assert(report.success); assert(std::filesystem::exists(p.root/"Builds/Test/BuildManifest.txt"));
    auto issues=doctor::ProjectDoctor().scan(p.root); assert(!issues.empty());
    auto snap=recovery::Recovery().snapshot(p.root,"test",err); assert(!snap.empty());
    world::WorldGenerator().generate(scene,"forest village"); assert(scene.entities().size()>3);
    testing::TestingCenter tc; tc.add("truth",[]{return true;}); auto tr=tc.run(); assert(tr.size()==1&&tr[0].passed);
    publish::StoreProfile sp; sp.appName="Test Game"; sp.publisher="Forge"; sp.description="Test"; sp.packageId="com.forge.test"; auto vp=publish::PublishCenter().validate(sp,p.root); assert(!vp.artifactDirectory.empty()); auto cr=compliance::ComplianceChecker().scan(sp,p.root); assert(cr.ready());
    auto langs=localization::LocalizationManager::supportedLanguages(); assert(langs.size()>=5); security::SecurityCenter().scan(p.root); workspace::WorkspaceProfile ws; assert(ws.name()=="Creator"); accessibility::AccessibilitySettings acc; assert(acc.subtitles); release::ReleaseRecord rr; rr.version="1.0.0"; assert(release::ReleaseManager().writeManifest(p.root,rr,err));
    {
        const auto home = root / "AccountHome"; std::filesystem::create_directories(home); const char* oldHome=std::getenv("HOME"); std::string oldHomeValue=oldHome?oldHome:""; setenv("HOME",home.string().c_str(),1);
        account::AccountService acct; assert(acct.load(err)); account::AccountProfile ap; ap.displayName="Creator"; ap.email="creator@forge.test"; ap.dateOfBirth="01/01/2000"; ap.country="Malaysia"; ap.timezone="Local"; ap.language="English"; assert(acct.signUp(ap,"password123",err)); account::AccountService acct2; assert(acct2.load(err)); assert(acct2.logIn("creator@forge.test","password123",err)); assert(!acct2.logIn("creator@forge.test","wrongpass",err));
        if(oldHome) setenv("HOME",oldHomeValue.c_str(),1); else unsetenv("HOME");
    }
    {
        assert(account::AccountPolicy::isValidDateOfBirth("01/01/2000"));
        assert(!account::AccountPolicy::isValidDateOfBirth("31/02/2000"));
        assert(account::AccountPolicy::birthdayMatches("01/01/2000", "01/01/2026"));
        assert(account::AccountPolicy::classify("01/01/2000") == account::AgeBracket::Adult);
        assert(account::AccountPolicy::birthdayGreeting("Creator").find("Happy Birthday") != std::string::npos);
        aiplatform::AIOrchestrator orchestrator; auto snapshot = orchestrator.inspect(p.root); assert(snapshot.projectFileValid && snapshot.files > 0);
        auto aiPlan = orchestrator.plan(aiplatform::Mode::Publish, "prepare release", snapshot); assert(aiPlan.size() >= 2); assert(orchestrator.permissionRequired(aiplatform::Permission::Publish));
        assert(aiplatform::AIOrchestrator::permissionName(aiplatform::Permission::Build) == "Build");
        publish::StoreProfile direct; direct.store=publish::Store::Direct; direct.appName="Test Game"; direct.publisher="Forge"; direct.description="Test"; direct.version="1.0.0"; direct.buildNumber="1";
        auto directPlan = publish::PublishCenter().validate(direct, p.root); bool hasBlocking=false; for(const auto& i:directPlan.issues) if(i.error) hasBlocking=true; assert(!hasBlocking);
        std::string manifestErr; assert(publish::PublishCenter().writeSubmissionManifest(direct, p.root, manifestErr)); assert(std::filesystem::exists(p.root/"Publishing/Direct Distribution/SubmissionManifest.json"));
    }
    fsys.shutdown(); std::filesystem::remove_all(root,ec); std::cout<<"ForgeCoreTests: ALL PASS\n"; return 0;
}
