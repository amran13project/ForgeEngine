#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include <cassert>
#include <filesystem>
#include <iostream>
int main(){
    auto root=std::filesystem::temp_directory_path()/"ForgeEngine_Phase1_Test"; std::filesystem::remove_all(root);
    forge::project::ProjectManager pm; forge::project::ProjectInfo p; std::string err;
    assert(pm.createProject("Test Game",root,"3D",p,err));
    assert(std::filesystem::exists(p.root/"ForgeProject.json"));
    assert(std::filesystem::exists(p.root/"Scenes/Main.forgeScene"));
    assert(std::filesystem::exists(p.root/"Assets"));
    forge::project::ProjectInfo opened; assert(pm.openProject(p.root,opened,err)); assert(opened.name=="Test_Game");
    forge::scene::Scene scene; assert(scene.load(p.root/"Scenes/Main.forgeScene",err)); assert(scene.selected()==nullptr); scene.select(1); assert(scene.selected()!=nullptr); scene.selected()->position.x=4; assert(scene.save(p.root/"Scenes/Main.forgeScene",err));
    std::filesystem::remove_all(root); std::cout<<"ForgeCoreTests: PASS\n";
}
