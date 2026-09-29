#include "input/InputSystem.h"
#include "gameplay/GameplayToolkit.h"
#include "animation/AnimationSystem.h"
#include "ui/WidgetTree.h"
#include "material/Material.h"
#include "prefab/Prefab.h"
#include "hotreload/HotReload.h"
#include "scene/Scene.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace forge;
int main(){
  input::InputSystem input; input.bind("jump",32); input.press(32); assert(input.pressed("jump")); input.release(32); assert(!input.pressed("jump")); input.setAxis("move",0.75f); assert(input.axis("move")==0.75f);
  gameplay::Health hp(100); hp.damage(35); hp.heal(10); assert(hp.current()==75 && hp.alive());
  gameplay::Inventory inv; assert(inv.add("wood",4)); assert(inv.add("wood",2)); assert(inv.count("wood")==6); assert(inv.remove("wood",3)); assert(inv.count("wood")==3);
  gameplay::Currency coins(10); assert(coins.spend(4)); assert(!coins.spend(10)); assert(coins.value()==6);
  gameplay::QuestLog q; q.add({"q1","Collect",0,3,false}); assert(q.advance("q1",2)); assert(!q.quests()[0].completed); assert(q.advance("q1",1)); assert(q.quests()[0].completed);
  gameplay::Timer timer; timer.start(1); timer.tick(.4f); assert(timer.running()); timer.tick(.7f); assert(timer.done());
  animation::Animator a; a.addClip({"Idle",1,true}); a.addState("Idle",{"Idle",1,""}); assert(a.play("Idle")); a.update(1.5f); assert(a.playing() && a.time()<1.0f);
  ui::UIContext ui; auto root=ui.create("Panel"); root->setName("HUD"); root->setRect({0,0,800,600}); root->add(ui.create("Button")); assert(root->children().size()==1 && root->name()=="HUD");
  material::Material m; m.name="PlayerMat"; m.metallic=.5f; assert(m.roughness>=0 && m.roughness<=1);
  scene::Scene scene; auto &e=scene.addCube(); e.position={2,3,4}; e.scale={2,1,1}; e.visible=false; e.locked=true; e.selected=true;
  auto dir=std::filesystem::temp_directory_path()/"ForgeFullSystems"; std::error_code ec; std::filesystem::remove_all(dir,ec); std::filesystem::create_directories(dir);
  [[maybe_unused]] prefab::PrefabStore pref; std::string err; auto pf=dir/"Player.prefab"; assert(pref.save(e,pf,err)); scene::Entity loaded; assert(pref.load(loaded,pf,err)); assert(loaded.name=="Cube" && loaded.position.x==2 && loaded.scale.x==2);
  hotreload::Watcher w; auto watched=dir/"watch.txt"; {std::ofstream o(watched);o<<"a";} assert(!w.changed(watched)); {std::ofstream o(watched);o<<"b";} assert(w.changed(watched));
  std::filesystem::remove_all(dir,ec); std::cout<<"ForgeFullSystemsTests: ALL PASS\n"; return 0;
}
