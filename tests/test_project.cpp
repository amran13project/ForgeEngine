#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
int main(){
    auto root=std::filesystem::temp_directory_path()/"ForgeEngine_Phase1_Test"; std::filesystem::remove_all(root);
    forge::project::ProjectManager pm; forge::project::ProjectInfo p; std::string err;
    assert(pm.createProject("Test Game",root,"3D",p,err));
    assert(std::filesystem::exists(p.root/"ForgeProject.json"));
    assert(std::filesystem::exists(p.root/"Scenes/Main.forgeScene"));
    assert(std::filesystem::exists(p.root/"Assets"));
    forge::project::ProjectInfo opened; assert(pm.openProject(p.root,opened,err)); assert(opened.name=="Test_Game");
    forge::scene::Scene scene; assert(scene.load(p.root/"Scenes/Main.forgeScene",err)); assert(scene.selected()==nullptr);
    scene.select(1); assert(scene.selected()!=nullptr); scene.selected()->position.x=4;
    scene.selected()->name="Camera \"Main\"\\Primary";
    assert(scene.save(p.root/"Scenes/Main.forgeScene",err));
    forge::scene::Scene reopenedScene;
    assert(reopenedScene.load(p.root/"Scenes/Main.forgeScene",err));
    assert(reopenedScene.selected()==nullptr);
    assert(reopenedScene.entities().front().name=="Camera \"Main\"\\Primary");
    assert(reopenedScene.entities().front().position.x==4.0f);

    const auto malformedScene=p.root/"Scenes/Malformed.forgeScene";
    { std::ofstream invalid(malformedScene); invalid << "{\"sceneVersion\":2,\"entities\":[{\"id\":1,\"name\":\"broken\""; }
    const auto previousEntities=reopenedScene.entities().size();
    assert(!reopenedScene.load(malformedScene,err));
    assert(!err.empty());
    assert(reopenedScene.entities().size()==previousEntities);
    { std::ofstream duplicate(malformedScene); duplicate << "{\"sceneVersion\":2,\"entities\":[{\"id\":1,\"name\":\"a\",\"type\":\"Mesh\"},{\"id\":1,\"name\":\"b\",\"type\":\"Mesh\"}]}"; }
    assert(!reopenedScene.load(malformedScene,err));
    assert(!err.empty());
    assert(reopenedScene.entities().size()==previousEntities);
    { std::ofstream duplicateProperty(malformedScene); duplicateProperty << "{\"sceneVersion\":2,\"entities\":[{\"id\":1,\"id\":2,\"name\":\"a\",\"type\":\"Mesh\"}]}"; }
    assert(!reopenedScene.load(malformedScene,err));
    assert(!err.empty());
    assert(reopenedScene.entities().size()==previousEntities);
    std::filesystem::remove_all(root); std::cout<<"ForgeProjectTests: PASS\n";
}
